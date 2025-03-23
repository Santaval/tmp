#ifndef HOLDER_HPP
#define HOLDER_HPP
#include "VServer.hpp"
#include "socks/Socket.h"
class Holder : public VServer {
  public:
		int start();
		int handleClientConnection();
	private:
		VSocket* serverSocket;
		int clientPort = 1234;
		int serverPort = 8080;
};
#endif