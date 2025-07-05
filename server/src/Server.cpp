#include <sys/socket.h>         // sockaddr_in
#include <arpa/inet.h>          // ntohs
#include <unistd.h>				// write, read
#include <cstring>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>
#include "sockets/BroadcastSocket.hpp"
#include "pig/includes/ProtocolParser.h"
#include "pig/includes/Message.h"
#include "./ServerBroadcastListener.hpp"

#include "Server.hpp"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <csignal>
#include <atomic>

static std::atomic<bool> g_terminate{false};
static Server* g_server_instance = nullptr;

static void handle_sigint(int) {
    std::cout << "\n[!] SIGINT received. Shutting down server..." << std::endl;
    g_terminate = true;
    if (g_server_instance) {
        g_server_instance->stopBroadcastThread();
        g_server_instance->closeListeningSocket();
        // Send shutdown broadcast before stopping threads
        std::string offMsg = "BEGIN/OFF/SERVIDOR/" + g_server_instance->getMyAddress() + "/" + std::to_string(g_server_instance->getClientPort()) + "/END";
        g_server_instance->sendBroadcast(offMsg);
    }
}

Server::Server(int clientPort, const std::string& myAddress) {
    if (!g_server_instance) g_server_instance = this;
    this->clientPort = clientPort;
    this->myAddress = myAddress;
    this->fs = new FileSystem("./disk.bin");
    this->fs->loadFilesFromDirectory("./resources");
    memset( &this->clientAddr, 0, sizeof( this->clientAddr ) );
    this->clientAddr.sin_family = AF_INET;
    this->clientAddr.sin_port = htons(this->clientPort);
}

int Server::handleClientConnection(char buffer[256], VSocket* socket) {
	std::cout << "[+] Handling client connection..." << std::endl;
	static std::string accumulatedRequest;
	accumulatedRequest += buffer;
	
	// Check if we have received a complete HTTP request
	if (accumulatedRequest.find("END") != std::string::npos) {
		printf("[+] Complete request received:\n%s\n", accumulatedRequest.c_str());
		
		if (accumulatedRequest.find("exit") != std::string::npos) {
			socket->sendTo("\0", 1, (void *) &this->clientAddr);
			accumulatedRequest.clear();
			return -1;
		}
		
		std::string deciphered = this->decipherRequest(const_cast<char*>(accumulatedRequest.c_str()));
		socket->sendTo(deciphered.c_str(), deciphered.length(), (void *) &this->clientAddr);
		socket->Close();
		// Clear the accumulated request after processing
		accumulatedRequest.clear();
	}
	return 0;
}

int Server::start(const char * address) {
    this->ipAddress = address;

    // init broadcast listener
	this->broadcastListener = new ServerBroadcastListener();
	if (this->broadcastListener == nullptr) {
		throw std::runtime_error("Fork::start - Failed to create broadcast listener");
		return -1;
	}
	// start broadcast listener in a separate thread
	if (this->broadcastListener->Thread::start() != 0) {
		std::cerr << "[-] Failed to start broadcast listener." << std::endl;
		return -1;
	}

    // Register signal handler for graceful shutdown
    std::signal(SIGINT, handle_sigint);

    // Start broadcast thread
    this->broadcastRunning = true;
    std::string broadcastMsg = "BEGIN/ON/SERVIDOR/"+ std::string(this->myAddress) + "/" + std::to_string(this->clientPort) + "/END";
    this->broadcastThread = std::thread([this, broadcastMsg]() {
       // while (this->broadcastRunning) {
        this->sendBroadcast(broadcastMsg);
           // std::this_thread::sleep_for(std::chrono::seconds(5));
       // }

    });

    // this->initSocket = new SSLSocket(true);
    this->initSocket = new Socket('s');
    if (this->initSocket == nullptr) {
        std::runtime_error( "Server::start" );
        return -1;
    }
    // prepare port to listen for connections
    if (this->initSocket->Bind(this->clientPort, "0.0.0.0") == 0) {

        if (this->initSocket->MarkPassive() == 0) {
            std::cout << "[+] Server listening on 0.0.0.0:" << this->clientPort << "\n";

            while (!g_terminate) { // loop for clients connections
                VSocket* newSock = this->initSocket->AcceptConnection(&this->clientAddr);
                if (g_terminate) break;
                if (newSock == nullptr) {
                    return -1;
                }

                printf("[+] New client connected from %s\n", 
                inet_ntoa(this->clientAddr.sin_addr));

                // set a child process to handle the incoming request
                pid_t childpid = fork();
                if (childpid == 0) {
                    // In child: set up SIGINT handler to exit immediately
                    std::signal(SIGINT, [](int){
                        std::cout << "\n[!] Child received SIGINT, exiting..." << std::endl;
                        exit(0);
                    });
                    // loop for taking requests for certain client
                    while (true) {
                        char buffer[250];
                        memset(buffer, '\0', sizeof(buffer));

                        int bytes = newSock->recvFrom((void*)buffer, 
                            sizeof(buffer), (void *) & this->clientAddr);
                        if ( bytes > 0) { // if not empty, manage response
                            buffer[bytes] = '\0';
                            if (this->handleClientConnection(buffer, newSock) == -1) {            
                                break;
                            }       
                        } else { // empty response from client
                            break;
                        }
                        // No need to check g_terminate in child, SIGINT will exit
                    }
                    printf("[+] Client disconnected\n");
                    exit(0); // Ensure child exits
                }
                newSock->Close();
            }
            // After loop, send SIGINT to all children
            kill(0, SIGINT);
            this->initSocket->Close();    
        } else { // Listening for connections failed
            return -1;
        }
    } else { // Binding socket failed
        return -1;
    }
    return 0;
}
std::string Server::decipherRequest(char* request) {
	std::string request_str(request);
	ProtocolParser parser;
	std::shared_ptr<Message> message = parser.parseMessage(request_str);
	std::cout << message->toString() << std::endl;
	// Handle different HTTP methods and paths
	if (message->getType() == MessageType::OBJECT_REQUEST) {
		std::string files = this->fs->listFiles();
		std::cout << "[+] Directory listing: " << files << std::endl;
		return parser.generateSuccessMessage(files);
		
	} else if (message->getType() == MessageType::RESOURCE_REQUEST) {
		std::string fileContent = this->fs->getFile(message->getDrawingName());

		std::string response = parser.generateSuccessMessage(fileContent);
		return response;
		
		
	}  else {	
		return parser.generateErrorMessage(404, "Not Found");
	}
}


Server::~Server() {
	this->broadcastRunning = false;
	if (this->broadcastThread.joinable()) {
		this->broadcastThread.join();
	}
	delete this->initSocket;
}

std::vector<std::string> Server::getBroadcastIPs() {
    std::vector<std::string> ips;
    std::ifstream file("./config/broadcast.txt");
    if (!file.is_open()) {
        std::cerr << "Could not open broadcast.txt" << std::endl;
        return ips;
    }
    std::string line;
    while (std::getline(file, line)) {
        // Remove possible carriage return or whitespace
        line.erase(line.find_last_not_of(" \r\n") + 1);
        if (!line.empty()) {
            ips.push_back(line);
        }
    }
    return ips;
}

void Server::sendBroadcast(const std::string& message) {
    std::vector<std::string> ips = getBroadcastIPs();
    for (const auto& ip : ips) {
        try {
            std::cout << "[+] Sending broadcast message to " << ip << ": " << message << std::endl;
            
            // Create a simple UDP socket manually for broadcasting
            int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd < 0) {
                std::cerr << "Error creating socket for broadcast" << std::endl;
                continue;
            }
            
            // Enable broadcast
            int broadcastEnable = 1;
            if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
                std::cerr << "Error enabling broadcast" << std::endl;
                close(sockfd);
                continue;
            }
            
            // Set up address
            struct sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(4321);
            addr.sin_addr.s_addr = inet_addr(ip.c_str());
            
            // Send the message
            ssize_t sent = sendto(sockfd, message.c_str(), message.length(), 0, (struct sockaddr*)&addr, sizeof(addr));
            if (sent < 0) {
                std::cerr << "Error sending broadcast message" << std::endl;
            }
            
            // Close the socket
            close(sockfd);
            
        } catch (const std::exception& e) {
            std::cerr << "Error sending broadcast to " << ip << ": " << e.what() << std::endl;
        }
    }
}
