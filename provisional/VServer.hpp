#ifndef VSERVER_HPP
#define VSERVER_HPP
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <list>
#include "socks/VSocket.hh"
#include "socks/Socket.hh"
class VServer {
  public:
		//VServer();
		~VServer();
		// int Bind(int port);
		// int Listen();
		// int Accept();
		virtual int start(const char * address) = 0;
		virtual int handleClientConnection() = 0;
	
	protected:
		int waitingPort;
		const char * ipAddress;
		const char * myAddress;
		std::list<const char *> ipList = {"127.0.0.1"};
		VSocket* initSocket;
		VSocket* waitingSocket;
};
#endif