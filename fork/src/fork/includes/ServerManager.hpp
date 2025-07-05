#pragma once

#include "ServerData.hpp"
#include <mutex>
#include <memory>

#define MAX_SERVERS 32  // Maximum number of servers we can handle

class ServerManager {
public:
    ServerManager();
    ~ServerManager();

    // Thread-safe operations
    bool addServer(ServerData* server);  // Returns false if array is full
    bool removeServer(const std::string& ip, int port);
    bool serverExists(const std::string& ip, int port);
    size_t getServerCount() const;  // Returns current number of servers
    ServerData** getServers(size_t& count);  // Returns array and sets count
    void clear();

private:
    ServerData* servers[MAX_SERVERS];
    size_t serverCount;
    mutable std::mutex mutex;  // Mutable so we can lock in const methods

    // Helper method to find server index
    int findServerIndex(const std::string& ip, int port) const;
};
