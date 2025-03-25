#include <sys/socket.h>         // sockaddr_in
#include <arpa/inet.h>          // ntohs
#include <unistd.h>		// write, read
#include <cstring>
#include <stdexcept>
#include <stdio.h>

#include "Server.hpp"

Server::Server(int holderPort, int waitingPort, const char* ip) {
	this->holderPort = holderPort; // 5051
	this->waitingPort = waitingPort; //8081
	this->myAddress = ip;
}
int Server::handleClientConnection() {
 return 1;
}

int Server::start(const char * address) {
    int st = -1;
    this->initSocket = new Socket('s');

    if (this->initSocket == nullptr) {
        std::runtime_error( "Server::start" );
		return st;
    }
    for (auto ip : this->ipList) {
		if (this->initSocket->MakeConnection(ip, this->holderPort) == 0) {
			printf("Connected to holder from %s\n", ip);
			st = this->initSocket->sendMsg("Hello from Server", 20);
			break;
		}
	}

	if (st == -1) {
		printf("No holder seems to be active.\n");
		// let server listen for holder msg
		this->waitingSocket = new Socket('s');

		if (this->waitingSocket->Bind(this->waitingPort) == 0) {

			if (this->waitingSocket->Listen() == 0) {
				printf("Listening for holder connections...\n");
				int newSock = this->waitingSocket->Accept();
				if (newSock == -1) {
					return -1;
				}
				char buffer[250];
				if (recv(newSock, buffer, 250, 0) != -1) {
					printf("Holder says: %s\n", buffer);
				}
			}
		}
	}printf("Now accepting requests from holder\n");
	while (1) {

	}

}