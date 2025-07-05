#include "ForkRoutes.hpp"
#include "Fork.hpp"
#include "http/HttpParser.h"
#include "pig/includes/ProtocolParser.h"
#include <string>
#include <iostream>
#include <algorithm>

void ForkRoutes::registerRoutes(Fork* fork) {
    fork->router->addRoute("GET", "/", [fork](const std::string& body, void* sock, const std::map<std::string, std::string>& params) {
        VSocket* socket = static_cast<VSocket*>(sock);
        HttpParser parser;
        std::string response = "<html><body><h1>Welcome to the Fork Server!</h1></body></html>";
        response = parser.createHttpResponse(response);
        socket->Write(response.c_str(), response.length());
    });

    fork->router->addRoute("GET", "/figures", [fork](const std::string& body, void* sock, const std::map<std::string, std::string>& params) {
        std::cout << "[+] Handling /figures request" << std::endl;
        VSocket* socket = static_cast<VSocket*>(sock);
        
        // search on servers data for figures
        std::string response = "<html><body><h1>Available Figures</h1><ul>";
        size_t serverCount;
        auto servers = fork->serverManager.getServers(serverCount);
        for (size_t i = 0; i < serverCount; ++i) {
            const auto& server = servers[i];
            if (server != nullptr) {
                for (const auto& figure : server->figures) {
                    response += "<li><a href=\"/figures/" + figure + "\">" + figure + "</a></li>";
                }
            }
        }
        response += "</ul></body></html>";
        HttpParser parser;
        std::string httpResponse = parser.createHttpResponse(response);
        std::cout << "[+] Sending response for /figures request: " << httpResponse << std::endl;
        socket->Write(httpResponse.c_str(), httpResponse.length());
        
    });

    fork->router->addRoute("GET", "/servers", [fork](const std::string& body, void* sock, const std::map<std::string, std::string>& params) {
        VSocket* socket = static_cast<VSocket*>(sock);
        std::string response = "[";
        bool first = true;
        size_t serverCount;
        auto servers = fork->serverManager.getServers(serverCount);
        for (size_t i = 0; i < serverCount; ++i) {
            const auto& server = servers[i];
            if (server != nullptr) {
                if (!first) response += ", ";
                response += "{\"ip\": \"" + server->ip + "\", \"port\": " + std::to_string(server->port);
                // Add figures for this server
                response += ", \"figures\": [";
                bool firstFig = true;
                for (const auto& fig : server->figures) {
                    if (!firstFig) response += ", ";
                    response += "\"" + fig + "\"";
                    firstFig = false;
                }
                response += "]}";
                first = false;
            }
        }
        response += "]";
        HttpParser parser;
        std::string httpResponse = parser.createHttpResponse(response);
        socket->Write(httpResponse.c_str(), httpResponse.length());
    });

    fork->router->addRoute("GET", "/figures/:filename", [fork](const std::string& body, void* sock, const std::map<std::string, std::string>& params) {
        VSocket* socket = static_cast<VSocket*>(sock);
        auto it = params.find("filename");
        std::string response;
        if (it != params.end()) {
            std::string filename = it->second;
            std::cout << "[+] Handling request for figure: " << filename << std::endl;
            
            // Find the server that has this figure
            size_t serverCount;
            auto servers = fork->serverManager.getServers(serverCount);
            for (size_t i = 0; i < serverCount; ++i) {
                const auto& server = servers[i];
                if (server != nullptr && std::find(server->figures.begin(), server->figures.end(), filename) != server->figures.end()) {
                    Socket* serverSocket = new Socket('s');
                    if (serverSocket->MakeConnection(server->ip.c_str(), server->port) == 0) {
                        std::string requestMessage = "BEGIN/GET/" + filename + "/END";
                        serverSocket->Write(requestMessage.c_str(), requestMessage.length());
                        char responseBuffer[1024];
                        std::string fullResponse;
                        while (true) {
                            int bytesRead = serverSocket->Read(responseBuffer, sizeof(responseBuffer) - 1);
                            if (bytesRead > 0) {
                                responseBuffer[bytesRead] = '\0';
                                fullResponse += std::string(responseBuffer, bytesRead);
                                if (fullResponse.find("/END") != std::string::npos) {
                                    break;
                                }
                            } else {
                                break;
                            }
                        }
                        if (!fullResponse.empty()) {
                            std::cout << "[DEBUG] Received response from server: " << fullResponse << std::endl;
                            ProtocolParser parser;
                            std::shared_ptr<Message> message = parser.parseMessage(fullResponse);
                            if (message->getType() == MessageType::SUCCESS) {
                                // Wrap ASCII/text response in <pre> tags for browser rendering
                                response = "<pre>" + message->getContent() + "</pre>";
                                std::cout << "[+] Successfully retrieved figure: " << filename << std::endl;
                            } else {
                                response = "<html><body><h1>Error: Figure not found</h1></body></html>";
                            }
                        } else {
                            response = "<html><body><h1>Error: No data received</h1></body></html>";
                        }
                        serverSocket->Close();
                    } else {
                        response = "<html><body><h1>Error: Could not connect to server</h1></body></html>";
                    }
                    break; // Stop after finding the first matching server
                }
            }
            


        } else {
            response = "<html><body><h1>Error: No filename provided</h1></body></html>";
        }
        HttpParser parser;
        std::string httpResponse = parser.createHttpResponse(response);
        std::cout << "[+] Sending response for figure request: " << httpResponse << std::endl;
        socket->Write(httpResponse.c_str(), httpResponse.length());
    });
}
