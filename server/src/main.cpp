#include <iostream>
#include <cstdlib>
#include "Server.hpp"

constexpr int DEFAULT_PORT = 5051;
constexpr const char* DEFAULT_IP = "127.0.0.1";

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [port] [ip_address]\n"
              << "  port: The port number to listen on (default: " << DEFAULT_PORT << ")\n"
              << "  ip_address: The IP address to bind to (default: " << DEFAULT_IP << ")\n";
}

int main(int argc, char* argv[]) {
    int port = DEFAULT_PORT;
    const char* ip = DEFAULT_IP;

    // Parse command line arguments
    if (argc > 1) {
        port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            std::cerr << "Error: Invalid port number. Must be between 1 and 65535.\n";
            printUsage(argv[0]);
            return 1;
        }
    }
    if (argc > 2) {
        ip = argv[2];
    }

    try {
        std::cout << "Starting server on " << ip << ":" << port << std::endl;
        
        // Create and start the server
        Server server(port, ip);
        if (server.start(ip) != 0) {
            std::cerr << "Error: Failed to start server\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}