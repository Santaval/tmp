#ifndef SERVER_HPP
#define SERVER_HPP
#include "VServer.hpp"
#include "socks/VSocket.hh"
class Server: public VServer {

  public:
		Server(int, int, const char*);
		~Server();
		int start(const char * address);
		int handleClientConnection();
	private:
		int holderPort;
};

#endif