#ifndef VSERVER_HPP
#define VSERVER_HPP
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <list>
#include "socks/VSocket.h"
class VServer {
  public:
		VServer();
		~VServer();
		// int Bind(int port);
		// int Listen();
		// int Accept();
		virtual int start() = 0;
		virtual int handleClientConnection() = 0;
	
	protected:
		int waitingPort;
		int otherPort;
		const char * ipAddress;
		const char * myAddress;
		std::list<const char *> ipList;
		VSocket* socket;
};
#endif