#ifndef VSERVER_HH
#define VSERVER_HHH
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <list>
#include <sys/types.h>
#include <unistd.h>
#include "sockets/VSocket.hh"
#include "sockets/Socket.hh"
class VServer {
  public:
		//VServer();
		~VServer();
		virtual int start(const char * address) = 0;
	
	protected:
		int waitingPort;
		const char * ipAddress;
		const char * myAddress;
		std::list<const char *> ipList = {"127.0.0.1"};
		VSocket* initSocket;
		VSocket* waitingSocket;
};
#endif