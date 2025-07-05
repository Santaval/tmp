#pragma once

#include "thread/includes/Thread.hpp"
#include "./VServer.hpp"
#include "sockets/VSocket.hpp"

#include <vector>

#define SERVER_BROADCAST_LISTENER_PORT 1234

class ServerBroadcastListener: public VServer, public Thread {
    public:
        ServerBroadcastListener();  // Take a reference instead of a pointer
        ~ServerBroadcastListener();
        int handleClientConnection(char buffer[256], VSocket* socket);
        int run() override;
        
    protected:
        int start(const char * address) override;
        void sendBroadcast(const std::string &message, const std::string &broadcastIp);
        struct sockaddr_in clientAddr;
        int clientPort;
        const char* ipAddress;
        int stop();
};