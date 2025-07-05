#ifndef SERVER_HPP
#define SERVER_HPP

#include "./includes/VServer.hpp"
#include "sockets/includes/VSocket.hpp"
#include "sockets/includes/SSLSocket.hpp"
#include "thread/includes/Thread.hpp"
#include "client/includes/Client.hpp"
#include "fork/includes/ForkBroadcastListener.hpp"
#include "ServerData.hpp"
#include "Router.hpp"
#include <vector>


class Fork: public VServer {

  public:
		/// @brief Constructor of the Fork Class
		/// @param clientPort port in which Fork listens for clients
		/// @param ip IP address of fork (if local, will be "127.0.0.1")
		Fork(int clientPort, const char* ip);
		
		/// @brief Destructor of Fork Class
		~Fork();

		/// @brief turns on the Server to wait for connections and then recieve requests
		/// @param address address for future connections to holder (now is equal to server ip)
		/// @return a flag for success or failure
		int start(const char * address) override;

		/// @brief identifies if client requests anything or exited the connection
		/// @param buffer request recieved from the client  
		/// @param socket socket through which server and client are communicating
		/// @return -1 for exit and 0 for request
		int handleClientConnection(char buffer[256], VSocket* socket);

		friend class ForkRoutes;

	protected:
		ServerManager serverManager;  // Direct member, not a pointer
		Router* router;
	private:
		struct sockaddr_in clientAddr;
		int clientPort;
		std::vector<std::string> getBroadcastIPs();
		void sendBroadcast(const std::string& message);
		ForkBroadcastListener* broadcastListener;
};

#endif