#ifndef SERVER_HPP
#define SERVER_HPP

#include "VServer.hpp"
#include "sockets/VSocket.hpp"
#include "sockets/SSLSocket.hpp"
#include "./ServerBroadcastListener.hpp"
#include "FileSystem.hpp"
#include <thread>
#include <atomic>
#include <fstream>
#include <sstream>

class Server: public VServer {

  public:
		/// @brief Constructor of the Server Class
		/// @param clientPort port in which Server listens for clients
		/// @param myAddress IP address where the server is running
		Server(int clientPort, const std::string& myAddress);
		
		/// @brief Destructor of Server Class
		~Server();

		/// @brief turns on the Server to wait for connections and then recieve requests
		/// @param address address for future connections to holder (now is equal to server ip)
		/// @return a flag for success or failure
		int start(const char * address) override;

		void sendBroadcast(const std::string& message);
		std::vector<std::string> getBroadcastIPs();
		

		/// @brief translates request from client, which is in HTTP
		/// @param  char* array of the client reuqest
		/// @return a string type of the response
		std::string decipherRequest(char*);

		/// @brief identifies if client requests anything or exited the connection
		/// @param buffer request recieved from the client  
		/// @param socket socket through which server and client are communicating
		/// @return -1 for exit and 0 for request
		int handleClientConnection(char buffer[256], VSocket* socket);

		void stopBroadcastThread() { broadcastRunning = false; }
		void closeListeningSocket() { if (initSocket) initSocket->Close(); }
		std::string getMyAddress() const { return myAddress; }
		int getClientPort() const { return clientPort; }

	private:
		struct sockaddr_in clientAddr;
		FileSystem* fs;
		int clientPort;
		std::string myAddress;
		std::thread broadcastThread;
		ServerBroadcastListener* broadcastListener{nullptr};
		std::atomic<bool> broadcastRunning{false};
};

#endif