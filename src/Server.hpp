#ifndef SERVER_HPP
#define SERVER_HPP
#include "VServer.hpp"
#include "socks/VSocket.h"
class Server: VServer {

  public:
		int start();
		int handleClientConnection();

	
};

#endif