#include <sys/socket.h>         // sockaddr_in
#include <arpa/inet.h>          // ntohs
#include <unistd.h>				// write, read
#include <cstring>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>

#include "Server.hpp"

Server::Server(int clientPort, const char* ip) {
	this->clientPort = clientPort; // 5051
	this->myAddress = ip;
	this->fs = new FileSystem("./disk.bin");
	this->fs->loadFilesFromDirectory("./resources");
	memset( &this->clientAddr, 0, sizeof( this->clientAddr ) );
	this->clientAddr.sin_family = AF_INET;
	this->clientAddr.sin_port = htons(this->clientPort);
}

int Server::handleClientConnection(char buffer[256], VSocket* socket) {
	static std::string accumulatedRequest;
	accumulatedRequest += buffer;
	
	// Check if we have received a complete HTTP request
	if (accumulatedRequest.find("\r\n\r\n") != std::string::npos) {
		printf("[+] Complete request received:\n%s\n", accumulatedRequest.c_str());
		
		if (accumulatedRequest.find("exit") != std::string::npos) {
			socket->sendTo("\0", 1, (void *) &this->clientAddr);
			accumulatedRequest.clear();
			return -1;
		}
		
		std::string deciphered = this->decipherRequest(const_cast<char*>(accumulatedRequest.c_str()));
		socket->sendTo(deciphered.c_str(), deciphered.length(), (void *) &this->clientAddr);
		
		// Clear the accumulated request after processing
		accumulatedRequest.clear();
	}
	return 0;
}

int Server::start(const char * address) {
	// this->initSocket = new SSLSocket(true);
	this->initSocket = new Socket('s');
	if (this->initSocket == nullptr) {
		std::runtime_error( "Server::start" );
		return -1;
	}
	// prepare port to listen for connections
	if (this->initSocket->Bind(this->clientPort, address) == 0) {

		if (this->initSocket->MarkPassive() == 0) {
			std::cout << "[+] Listening for client connections...\n";

			while (true) {	// loop for clients connections
				VSocket* newSock = this->initSocket->AcceptConnection(&this->clientAddr);
				if (newSock == nullptr) {
					return -1;
				}

				printf("[+] New client connected from %s:\n", 
				inet_ntoa(this->clientAddr.sin_addr));

				// set a child process to handle the incoming request
				pid_t childpid = fork();
				if (childpid == 0) {

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
					}
					printf("[+] Client disconnected\n");
				}
				newSock->Close();
			}
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
	std::cout << "[+] Request: " << request_str << std::endl;

	// Find the first line (request line)
	size_t firstLineEnd = request_str.find("\r\n");
	if (firstLineEnd == std::string::npos) {
		return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\nInvalid request format";
	}
	
	std::string requestLine = request_str.substr(0, firstLineEnd);
	
	// Parse method and path from request line
	size_t firstSpace = requestLine.find(" ");
	if (firstSpace == std::string::npos) {
		return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\nInvalid request format";
	}
	std::string method = requestLine.substr(0, firstSpace);
	
	size_t secondSpace = requestLine.find(" ", firstSpace + 1);
	if (secondSpace == std::string::npos) {
		return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\nInvalid request format";
	}
	std::string path = requestLine.substr(firstSpace + 1, secondSpace - firstSpace - 1);
	
	std::cout << "[+] Method: " << method << std::endl;
	std::cout << "[+] Path: " << path << std::endl;
	
	std::string response;
	std::string contentType = "text/plain; charset=UTF-8";
	int statusCode = 200;
	
	// Handle different HTTP methods and paths
	if (method == "GET") {
		if (path == "/resources") {
			// Return directory listing
			response = "--resources\n---asthetic.txt\n---chillGuy.txt\n---lakshmi.txt\n---lakshmi.txt\n---valorant.txt\n";
		} else if (path.find("/resources/") == 0) {
			// Handle specific resource request
			std::string filename = path.substr(11); // Remove "/resources/"
			response = this->fs->getFile(filename.c_str());
			
			// Determine content type based on file extension
			if (filename.find(".txt") != std::string::npos) {
				contentType = "text/plain; charset=UTF-8";
			} else if (filename.find(".html") != std::string::npos) {
				contentType = "text/html; charset=UTF-8";
			} else if (filename.find(".json") != std::string::npos) {
				contentType = "application/json; charset=UTF-8";
			}
		} else {
			response = "404 Not Found: " + method + " " + path;
			statusCode = 404;
		}
	} else {	
		response = "404 Not Found: " + method + " " + path;
		statusCode = 404;
	}
	
	// Build HTTP response
	std::string httpResponse = "HTTP/1.1 " + std::to_string(statusCode) + " " + 
							 (statusCode == 200 ? "OK" : "Not Found") + "\r\n";
	httpResponse += "Content-Type: " + contentType + "\r\n";
	httpResponse += "Content-Length: " + std::to_string(response.length()) + "\r\n";
	httpResponse += "Connection: close\r\n";
	httpResponse += "\r\n";
	httpResponse += response + "\r\n\r\n";
	
	return httpResponse;
}


Server::~Server() {
	delete this->initSocket;
}