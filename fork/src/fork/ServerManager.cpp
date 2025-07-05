#include "includes/ServerManager.hpp"
#include <iostream>
#include <cstring>

ServerManager::ServerManager() : serverCount(0) {
    // Initialize all pointers to nullptr
    std::memset(servers, 0, sizeof(servers));
}

ServerManager::~ServerManager() {
    clear();
}

int ServerManager::findServerIndex(const std::string& ip, int port) const {
    for (size_t i = 0; i < serverCount; ++i) {
        if (servers[i] && servers[i]->ip == ip && servers[i]->port == port) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool ServerManager::addServer(ServerData* server) {
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << "[+] Attempting to add server: " << server->ip << ":" << server->port << std::endl;
    
    // First check if server already exists
    int existingIndex = findServerIndex(server->ip, server->port);
    if (existingIndex >= 0) {
        std::cout << "[-] Server " << server->ip << ":" << server->port << " already exists at index " << existingIndex << std::endl;
        delete server;  // Delete the duplicate
        return false;
    }

    if (serverCount >= MAX_SERVERS) {
        std::cerr << "[-] Cannot add server: maximum capacity reached" << std::endl;
        delete server;
        return false;
    }
    
    // Find first empty slot (just in case there are gaps)
    size_t insertIndex = serverCount;
    for (size_t i = 0; i < MAX_SERVERS; ++i) {
        if (servers[i] == nullptr) {
            insertIndex = i;
            break;
        }
    }
    
    std::cout << "[+] Adding server: " << server->ip << ":" << server->port << " at index " << insertIndex << std::endl;
    servers[insertIndex] = server;
    if (insertIndex == serverCount) {
        ++serverCount;
    }
    
    // Debug: Print entire server array
    std::cout << "[Debug] Current server array state:" << std::endl;
    for (size_t i = 0; i < MAX_SERVERS; ++i) {
        if (servers[i]) {
            std::cout << "    [" << i << "] " << servers[i]->ip << ":" << servers[i]->port << std::endl;
        } else {
            std::cout << "    [" << i << "] nullptr" << std::endl;
        }
    }
    std::cout << "    Total count: " << serverCount << std::endl;
    
    return true;
}

bool ServerManager::removeServer(const std::string& ip, int port) {
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << "[+] Removing server: " << ip << ":" << port << std::endl;
    
    // Print current state before removal
    std::cout << "[Debug] Before removal - server array state:" << std::endl;
    for (size_t i = 0; i < serverCount; ++i) {
        if (servers[i]) {
            std::cout << "    [" << i << "] " << servers[i]->ip << ":" << servers[i]->port << std::endl;
        } else {
            std::cout << "    [" << i << "] nullptr" << std::endl;
        }
    }
    
    int index = findServerIndex(ip, port);
    if (index >= 0) {
        // Store the server to be deleted
        ServerData* toDelete = servers[index];
        
        // Shift all elements after index one position to the left
        for (size_t i = index; i < serverCount - 1; ++i) {
            servers[i] = servers[i + 1];
        }
        
        // Clear the last element and update count
        servers[serverCount - 1] = nullptr;
        --serverCount;
        
        // Now it's safe to delete the server
        delete toDelete;
        
        // Validate array state
        for (size_t i = 0; i < serverCount; ++i) {
            if (servers[i] == nullptr) {
                std::cerr << "ERROR: Null server found at index " << i << " when count is " << serverCount << std::endl;
                // Fix the count to match reality
                serverCount = i;
                break;
            }
        }
        
        // Print state after removal
        std::cout << "[Debug] After removal - server array state:" << std::endl;
        for (size_t i = 0; i < serverCount; ++i) {
            if (servers[i]) {
                std::cout << "    [" << i << "] " << servers[i]->ip << ":" << servers[i]->port << std::endl;
            } else {
                std::cout << "    [" << i << "] nullptr" << std::endl;
            }
        }
        std::cout << "    Total count: " << serverCount << std::endl;
        
        return true;
    }
    return false;
}

bool ServerManager::serverExists(const std::string& ip, int port) {
    std::lock_guard<std::mutex> lock(mutex);
    return findServerIndex(ip, port) >= 0;
}

size_t ServerManager::getServerCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    return serverCount;
}

ServerData** ServerManager::getServers(size_t& count) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Validate array state first
    size_t validCount = 0;
    for (size_t i = 0; i < serverCount; ++i) {
        if (servers[i] != nullptr) {
            // If we find a gap, compact the array
            if (validCount < i) {
                servers[validCount] = servers[i];
                servers[i] = nullptr;
            }
            ++validCount;
        }
    }
    
    // Update count if it doesn't match reality
    if (validCount != serverCount) {
        std::cout << "[Debug] Fixing server count from " << serverCount << " to " << validCount << std::endl;
        serverCount = validCount;
    }
    
    count = serverCount;
    
    // print servers
    std::cout << "[+] Current servers (Total: " << serverCount << "):" << std::endl;
    for (size_t i = 0; i < serverCount; ++i) {
        if (servers[i]) {
            std::cout << "    [" << i << "] " << servers[i]->ip << ":" << servers[i]->port << std::endl;
        } else {
            std::cerr << "ERROR: Null server found at index " << i << " when count is " << serverCount << std::endl;
            // Fix the count to match reality
            serverCount = i;
            count = i;
            break;
        }
    }
    
    return servers;
}

void ServerManager::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    for (size_t i = 0; i < serverCount; ++i) {
        if (servers[i]) {
            delete servers[i];
            servers[i] = nullptr;
        }
    }
    serverCount = 0;
}
