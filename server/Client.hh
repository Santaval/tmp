#ifndef CLIENT_HH
#define CLIENT_HH

#include <stdexcept>
#include <sys/socket.h>         // sockaddr_in
#include <arpa/inet.h>          // ntohs
#include <unistd.h>				// write, read
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <stdio.h>

#include "sockets/VSocket.hh"
#include "sockets/Socket.hh"
#include "sockets/SSLSocket.hh"

#define MAX_SIZE 1024

class Client {
  public:
	/// @brief Constructor of Client Class
	/// @param serverPort in which server is listening for connections
	/// @param ip address of client pc
	Client(int serverPort, const char * ip);

	/// @brief Destructor of class
	~Client();

	/// @brief inits connections of client with server
	/// @param serverAddr address of server to which clients will connect
	void start(const char* serverAddr);

	/// @brief asks for input to send to server, waits for responses
	/// @return 1 for exit, -1 for server disconnection, 0 for continuing
	int makeRequests();

	/// @brief translates user input to HTTP
	/// @param data input of client
	/// @return translated array of input or invalid
	const char* buildRequest( std::string data);

  private:
	int serverPort;
	const char * myAddress;
	VSocket* initSocket;

};
#endif