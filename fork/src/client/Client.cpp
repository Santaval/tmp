#include "Client.hpp"
#include <sstream>
#include <regex>
#include <stdexcept>
#include <iostream>

#include "pig/includes/ProtocolParser.h"



Client::Client() : Socket('S', false) {
}

Client::~Client() {
    Close();
}

std::string Client::parseUrl(const std::string& url, std::string& host, std::string& path) {
    std::regex url_regex("(?:http://)?([^/:]+)(?::(\\d+))?(/.*)?");
    std::smatch matches;
    
    if (std::regex_match(url, matches, url_regex)) {
        host = matches[1].str();
        path = matches[3].matched ? matches[3].str() : "/";
        return matches[2].matched ? matches[2].str() : "80";
    }
    
    throw std::invalid_argument("Invalid URL format");
}

std::string Client::buildHttpRequest(const std::string& method, const std::string& path,
                                   const std::map<std::string, std::string>& headers,
                                   const std::string& body) {
    std::stringstream request;
    
    // Request line
    request << method << " " << path << " HTTP/1.1\r\n";
    
    // Headers
    for (const auto& header : headers) {
        request << header.first << ": " << header.second << "\r\n";
    }
    
    // Add Content-Length if there's a body
    if (!body.empty()) {
        request << "Content-Length: " << body.length() << "\r\n";
    }
    
    // End of headers
    request << "\r\n";
    
    // Body
    if (!body.empty()) {
        request << body;
    }
    
    return request.str();
}

std::string Client::readHttpResponse() {
    std::string response;
    char buffer[4096];
    size_t bytesRead;
    bool headersComplete = false;
    size_t contentLength = 0;
    size_t bodyStart = 0;
    
    while ((bytesRead = Read(buffer, sizeof(buffer))) > 0) {
        response += std::string(buffer, bytesRead);   
    }
    
    std::cout << "[+] Complete response: " << response << std::endl;
    return response;
}

bool Client::connectToHost(const std::string& host, int port) {
    return MakeConnection(host.c_str(), port) == 0;
}

std::string Client::send(const std::string& url, std::string& message, 
                        MessageType type) {
    std::string host, path;
    std::string portStr = parseUrl(url, host, path);
    int port = std::stoi(portStr);
    
    if (!connectToHost(host, port)) {
        throw std::runtime_error("Failed to connect to host");
    }
    
    ProtocolParser parser;
    Message msg;
    msg.setType(type);
    msg.setContent(message);
    std::string body = parser.generateMessage(std::make_shared<Message>(msg));
    Write(body.c_str(), body.length());
    const std::string response = readHttpResponse();
    return response;
}
