#include <sys/socket.h>         // sockaddr_in
#include <arpa/inet.h>          // ntohs
#include <unistd.h>				// write, read
#include <cstring>
#include <stdexcept>
#include <stdio.h>

#include "Server.hh"

Server::Server(int clientPort, const char* ip) {
	this->clientPort = clientPort; // 5051
	this->myAddress = ip;

	memset( &this->clientAddr, 0, sizeof( this->clientAddr ) );
	this->clientAddr.sin_family = AF_INET;
	this->clientAddr.sin_port = htons(this->clientPort);
}

int Server::handleClientConnection(char buffer[256], VSocket* socket) {
	printf("[+] Request from client: %s\n", buffer);

	if (strcmp(buffer, "exit") == 0) {
		socket->sendTo("\0", 1, (void *) &  this->clientAddr);
		return -1;
	}
	std::string deciphered = this->decipherRequest(buffer);
	socket->sendTo(deciphered.c_str(), deciphered.length(), (void *) &this->clientAddr);
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
	// this part should be done by making use of the file system (as an attribute)
	if (request_str == "GET /resources") {
		// return directory beautifully placed
		std::string response = "--resources\n---asthetic.txt\n---chillGuy.txt\n---lakshmi.txt\n---MichaelMouse.txt\n---valorant.txt\n";
		return response;
	} else {
		size_t found = request_str.find("GET /resources/");
		if (found != std::string::npos) {
			request_str = request_str.substr(5, request_str.length()- 5);

		}
	}
	return request_str;
}

