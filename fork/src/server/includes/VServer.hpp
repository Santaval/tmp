#ifndef VSERVER_HH
#define VSERVER_HH

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>  // for sockaddr_in
#include <list>
#include <sys/types.h>
#include <unistd.h>
#include "sockets/includes/VSocket.hpp"
#include "sockets/includes/Socket.hpp"

class VServer {
  public:
		VServer() {
            ipList.push_back("127.0.0.1");
        }
		virtual ~VServer();
		virtual int start(const char * address) = 0;
	
	protected:
		int waitingPort;
		const char * ipAddress;
		const char * myAddress;
		std::list<const char *> ipList;
		VSocket* initSocket;
		VSocket* waitingSocket;
};

#endif // VSERVER_HH