#include "Client.hpp"
#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <server_ip> <port>\n"
              << "Example: " << programName << " 127.0.0.1 8080\n\n"
              << "Available commands:\n"
              << "  get <path> [header:value]...\n"
              << "  post <path> <body> [header:value]...\n"
              << "  put <path> <body> [header:value]...\n"
              << "  delete <path> [header:value]...\n"
              << "  help - Show this help message\n"
              << "  exit - Exit the program\n\n"
              << "Examples:\n"
              << "  get /api/users\n"
              << "  get /api/users User-Agent:MyClient/1.0\n"
              << "  post /api/users '{\"name\":\"John\"}' Content-Type:application/json\n";
}

std::map<std::string, std::string> parseHeaders(const std::vector<std::string>& args, size_t startIndex) {
    std::map<std::string, std::string> headers;
    for (size_t i = startIndex; i < args.size(); ++i) {
        size_t colonPos = args[i].find(':');
        if (colonPos != std::string::npos) {
            std::string key = args[i].substr(0, colonPos);
            std::string value = args[i].substr(colonPos + 1);
            headers[key] = value;
        }
    }
    return headers;
}

std::vector<std::string> splitCommand(const std::string& input) {
    std::vector<std::string> args;
    std::stringstream ss(input);
    std::string arg;
    bool inQuotes = false;
    std::string currentArg;

    for (char c : input) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ' ' && !inQuotes) {
            if (!currentArg.empty()) {
                args.push_back(currentArg);
                currentArg.clear();
            }
        } else {
            currentArg += c;
        }
    }
    
    if (!currentArg.empty()) {
        args.push_back(currentArg);
    }
    
    return args;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printUsage(argv[0]);
        return 1;
    }

    std::string serverIP = argv[1];
    int port;
    try {
        port = std::stoi(argv[2]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid port number" << std::endl;
        return 1;
    }

    std::string input;
    
    std::cout << "HTTP Client - Connected to " << serverIP << ":" << port << "\n"
              << "Type 'help' for available commands\n";
    
    while (true) {
        Client client;  
        std::cout << "\n> ";
        std::getline(std::cin, input);
        
        if (input.empty()) continue;
        
        std::vector<std::string> args = splitCommand(input);
        if (args.empty()) continue;
        
        std::string command = args[0];
        
        try {
            if (command == "exit") {
                break;
            } else if (command == "help") {
                printUsage(argv[0]);
            } else if (command == "get" && args.size() >= 2) {
                std::map<std::string, std::string> headers = parseHeaders(args, 2);
                std::string response = client.GET("http://" + serverIP + ":" + std::to_string(port) + args[1], headers);
                std::cout << "Response:\n" << response << std::endl;
            } else if (command == "post" && args.size() >= 3) {
                std::map<std::string, std::string> headers = parseHeaders(args, 3);
                std::string response = client.POST("http://" + serverIP + ":" + std::to_string(port) + args[1], args[2], headers);
                std::cout << "Response:\n" << response << std::endl;
            } else if (command == "put" && args.size() >= 3) {
                std::map<std::string, std::string> headers = parseHeaders(args, 3);
                std::string response = client.PUT("http://" + serverIP + ":" + std::to_string(port) + args[1], args[2], headers);
                std::cout << "Response:\n" << response << std::endl;
            } else if (command == "delete" && args.size() >= 2) {
                std::map<std::string, std::string> headers = parseHeaders(args, 2);
                std::string response = client.DELETE("http://" + serverIP + ":" + std::to_string(port) + args[1], headers);
                std::cout << "Response:\n" << response << std::endl;
            } else {
                std::cout << "Invalid command or missing arguments. Type 'help' for usage.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    
    return 0;
}