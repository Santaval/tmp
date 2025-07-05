#pragma once

#include <vector>
#include <string>
struct ServerData
{
    int port;          // Port number for the server
    std::string ip;    // IP address of the server
    std::vector<std::string> figures; // List of connected clients
};
