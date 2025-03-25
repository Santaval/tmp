#include <sys/socket.h>         // sockaddr_in
#include <arpa/inet.h>          // ntohs
#include <unistd.h>		// write, read
#include <cstring>
#include <stdexcept>
#include <stdio.h>

#include "Holder.hpp"

Holder::Holder(int serverPort, int waitingPort, int clientPort, const char* ip) {
	this->serverPort = serverPort; // 8081
	this->waitingPort = waitingPort; // 5051
	this->clientPort = clientPort;
	this->myAddress = ip;
}
int Holder::handleClientConnection() {
	// pending
	return 1;
}

int Holder::start(const char * address) {
	int st = -1;
	this->initSocket = new Socket('s');
	if (this->initSocket == nullptr) {
		std::runtime_error( "Holder::start" );
		return st;
	}
	// should try to connect first to Server port
	for (auto ip : this->ipList) {
		if (this->initSocket->MakeConnection(ip, this->serverPort) == 0) {
			printf("Connected to server from %s\n", ip);
			st = this->initSocket->sendMsg("Hello from Holder", 20);
			break;
		}
	}
	if (st == -1) {
		printf("No server seems to be active to connect with.\n");
		// let holder listen for server msg
		this->waitingSocket = new Socket('s');
		if (this->waitingSocket->Bind(this->waitingPort) == 0) {

			if (this->waitingSocket->Listen() == 0) {
				printf("Listening for server connections...\n");

				int newSock = this->waitingSocket->Accept();
				if (newSock ==-1) {
					return -1;
				}
				char buffer[250];
				if (recv(newSock, buffer, 250, 0) != -1) {
					printf("Server says: %s\n", buffer);
				}
			}
			
		} 
		
	}
	// when succesfully connected to server, waits for connections from clients
	this->connectClients();
	// toDo: acept()
	return 1;
}

int Holder::connectClients() {
	this->clientSocket = new Socket('s');
	if (this->clientSocket->Bind(this->clientPort) == 0) {
		printf("Client port binded succesfully\n");
		if (this->clientSocket->Listen() == 0) {
				printf("Listening for client connections...\n");
				// toDo: accept()
				this->clientSocket->Accept();
				this->handleClientConnection();
			}
	}
	return -1;
}