#pragma once

#include "thread/includes/Thread.hpp"
#include "server/includes/VServer.hpp"
#include "sockets/includes/VSocket.hpp"
#include "ServerData.hpp"
#include "ServerManager.hpp"

#include <vector>

#define FORK_BROADCAST_LISTENER_PORT 4321

class ForkBroadcastListener: public VServer, public Thread {
    public:
        ForkBroadcastListener(ServerManager& serverManager);  // Take a reference instead of a pointer
        ~ForkBroadcastListener();
        int handleClientConnection(char buffer[256], VSocket* socket);
        int run() override;
        
    protected:
        int start(const char * address) override;
        struct sockaddr_in clientAddr;
        int clientPort;
        const char* ipAddress;
        ServerManager& serverManager;  // Hold a reference instead of a pointer
        int stop();
};