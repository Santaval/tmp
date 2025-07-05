#include "./includes/ForkBroadcastListener.hpp"
#include "sockets/includes/BroadcastSocket.hpp"
#include "pig/includes/ProtocolParser.h"
#include "pig/includes/Message.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

ForkBroadcastListener::ForkBroadcastListener(ServerManager& serverManager) 
    : serverManager(serverManager) {
    this->clientPort = FORK_BROADCAST_LISTENER_PORT;
    this->ipAddress = "127.0.0.1";
    this->initSocket = nullptr;
}

ForkBroadcastListener::~ForkBroadcastListener() {
    this->stop();
}

int ForkBroadcastListener::start(const char * /* address */) {
    try {
        this->initSocket = new BroadcastSocket(); // Use BroadcastSocket for UDP broadcast
        if (this->initSocket == nullptr) {
            throw std::runtime_error("ForkBroadcastListener::start - Failed to create broadcast socket");
        }
        static_cast<BroadcastSocket*>(this->initSocket)->EnableBroadcast();

        // Bind to all interfaces to receive broadcasts
        if (this->initSocket->Bind(this->clientPort, "0.0.0.0") == 0) {
            std::cout << "[+] ForkBroadcastListener listening for broadcasts on 0.0.0.0:" << this->clientPort << std::endl;
            
            // Receive loop
            while (true) {
                char buffer[1024];
                struct sockaddr_in clientAddr;
                memset(&clientAddr, 0, sizeof(clientAddr));
                
                size_t bytes = this->initSocket->recvFrom(buffer, sizeof(buffer), &clientAddr);
                if (bytes > 0) {
                    buffer[bytes] = '\0';
                    std::cout << "[UDP] Message: " << buffer << std::endl;
                    
                    // Handle the broadcast message here
                    // You can add your broadcast handling logic
                    this->handleClientConnection(buffer, this->initSocket);
                }
            }
        } else {
            throw std::runtime_error("Failed to bind broadcast socket");
        }
    } catch (const std::exception& e) {
        std::cerr << "[-] Error: " << e.what() << std::endl;
        if (this->initSocket) {
            this->initSocket->Close();
            delete this->initSocket;
            this->initSocket = nullptr;
        }
        return -1;
    }
    
    return 0;
}

int ForkBroadcastListener::stop() {
    if (this->initSocket) {
        this->initSocket->Close();
        delete this->initSocket;
        this->initSocket = nullptr;
    }
    return 0;
}


int ForkBroadcastListener::handleClientConnection(char buffer[256], VSocket* /* socket */) {
    ProtocolParser parser;
   std::shared_ptr<Message> message = parser.parseMessage(buffer);

    // Process the message as needed
    if (message->getType() == MessageType::ANNOUNCEMENT) {
        // Check if server already exists using thread-safe method
        if (this->serverManager.serverExists(message->getIP(), message->getPort())) {
            return 0; // Server already exists, no need to add again
        }

        ServerData* serverData = new ServerData();
        serverData->ip = message->getIP();
        serverData->port = message->getPort();
        this->serverManager.addServer(serverData);

        // send objects request to the new server
        std::cout << "[+] New server announcement from " << message->getIP() 
                  << ":" << message->getPort() << std::endl;
        std::cout << "[+] Sending OBJECTS request to " << message->getIP()
                  << ":" << message->getPort() << std::endl;
        Socket* serverSocket = new Socket('s');
        if (serverSocket->MakeConnection(message->getIP().c_str(), message->getPort()) == 0) {
            std::string requestMessage = "BEGIN/OBJECTS/END";
            serverSocket->Write(requestMessage.c_str(), requestMessage.length());
            char responseBuffer[1024];
            int bytesRead = serverSocket->Read(responseBuffer, sizeof(responseBuffer) - 1);
            if (bytesRead > 0) {
                responseBuffer[bytesRead] = '\0';
                std::cout << "[+] Received response from new server: " << responseBuffer << std::endl;
                // You can process the response further if needed
                ProtocolParser responseParser;
                std::shared_ptr<Message> responseMessage = responseParser.parseMessage(std::string(responseBuffer, bytesRead));
                if (responseMessage->getType() == MessageType::SUCCESS) {
                    std::cout << "[+] Successfully received objects from new server." << std::endl;
                    // You can add logic to handle the received objects
                   std::string objects = responseMessage->getContent();
                    // Objects separated by \n 
                    std::cout << "[+] Objects received: " << objects << std::endl;
                   
                    // aplit by \n and add to serverData
                    size_t pos = 0;
                    while ((pos = objects.find('\n')) != std::string::npos) {
                        std::string object = objects.substr(0, pos);
                        serverData->figures.push_back(object);
                        objects.erase(0, pos + 1);
                    }

                } else {
                    std::cerr << "[-] Error receiving objects from new server: " 
                              << responseMessage->getErrorMessage() << std::endl;
                }
            }
            serverSocket->Close();
        } else {
            std::cerr << "[-] Failed to connect to new server: " << message->getIP() 
                      << ":" << message->getPort() << std::endl;
        }


    } else if (message->getType() == MessageType::DISCONNECTION) {
        std::cout << "[+] Disconnection from " << message->getIP() 
                  << ":" << message->getPort() << std::endl;
                  
        // Remove the server using thread-safe method
        if (this->serverManager.removeServer(message->getIP(), message->getPort())) {
            std::cout << "[+] Server " << message->getIP() 
                      << ":" << message->getPort() << " removed from the list." << std::endl;
        }

        // Print the remaining servers
        std::cout << "[+] Remaining servers:" << std::endl;
        size_t serverCount;
        auto servers = this->serverManager.getServers(serverCount);
        for (size_t i = 0; i < serverCount; ++i) {
            const auto& server = servers[i];
            std::cout << "    - " << server->ip << ":" << server->port << std::endl;
        }
    } else {
        std::cout << "[+] Other message type received." << std::endl;
    }

    // You can add your logic to process the broadcast message
    return 0; // Return 0 to indicate success
}


int ForkBroadcastListener::run() {
    return this->start(this->ipAddress);
}