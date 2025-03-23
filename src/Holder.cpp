#include <sys/socket.h>         // sockaddr_in
#include <arpa/inet.h>          // ntohs
#include <unistd.h>		// write, read
#include <cstring>
#include <stdexcept>
#include <stdio.h>

#include "Holder.hpp"

int Holder::handleClientConnection() {

}

int Holder::start() {
	int st = -1;
	this->socket = new Socket('s');
	if (this->socket == nullptr) {
		std::runtime_error( "Holder::start" );
		return st;
	}
	// should try to connect first to Server port
	for (auto ip : this->ipList) {
		if (this->socket->MakeConnection("127.0.0.1", this->serverPort) == 0) {
			printf("Connected to server from %s\n", ip);
			st = this->socket->sendMsg("Hello from Holder", 20);
			break;
		}
	}
	if (st == -1) {
		printf("No server seems to be active to connect with.\n");
		// let port listen for server msg
		if (this->socket->Bind(this->serverPort) == 0) {
			// printf("Port binded succesfully\n");
			if (this->socket->Listen() == 0) {
				printf("Listening for server connections...\n");
				// toDo: accept()
			}
			
		} 
		
	}
	// when succesfully connected to server, waits for connections from clients
	this->socket->Bind(this->clientPort);
	// toDo: acept()
}