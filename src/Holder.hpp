#ifndef HOLDER_HPP
#define HOLDER_HPP
#include "VServer.hpp"
#include "socks/Socket.hh"
class Holder : public VServer {
  public:
		Holder(int, int, int, const char*);
		~Holder();
		int start(const char * address);
		int handleClientConnection();
		int connectClients();
	private:
		VSocket* clientSocket;
		int clientPort = 1234;
		int serverPort = 8080;
};
#endif