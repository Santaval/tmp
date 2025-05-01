#include "Client.hpp"
#include <sstream>
#include <regex>
#include <stdexcept>
#include <iostream>

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
    
    while ((bytesRead = Read(buffer, sizeof(buffer))) > 0) {
        response.append(buffer, bytesRead);
        
        // Check if we've received the complete response
        if (response.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    }
    // close the connection
    return response;
}

bool Client::connectToHost(const std::string& host, int port) {
    return MakeConnection(host.c_str(), port) == 0;
}

std::string Client::GET(const std::string& url, const std::map<std::string, std::string>& headers) {
    std::string host, path;
    std::string portStr = parseUrl(url, host, path);
    int port = std::stoi(portStr);
    
    if (!connectToHost(host, port)) {
        throw std::runtime_error("Failed to connect to host");
    }
    
    std::string request = buildHttpRequest("GET", path, headers);
    Write(request.c_str());
    const std::string response = readHttpResponse();
    return response;
}

std::string Client::POST(const std::string& url, const std::string& body,
                        const std::map<std::string, std::string>& headers) {
    std::string host, path;
    std::string portStr = parseUrl(url, host, path);
    int port = std::stoi(portStr);
    
    if (!connectToHost(host, port)) {
        throw std::runtime_error("Failed to connect to host");
    }
    
    std::string request = buildHttpRequest("POST", path, headers, body);
    Write(request.c_str());
    
    return readHttpResponse();
}

std::string Client::PUT(const std::string& url, const std::string& body,
                       const std::map<std::string, std::string>& headers) {
    std::string host, path;
    std::string portStr = parseUrl(url, host, path);
    int port = std::stoi(portStr);
    
    if (!connectToHost(host, port)) {
        throw std::runtime_error("Failed to connect to host");
    }
    
    std::string request = buildHttpRequest("PUT", path, headers, body);
    Write(request.c_str());
    
    return readHttpResponse();
}

std::string Client::DELETE(const std::string& url, const std::map<std::string, std::string>& headers) {
    std::string host, path;
    std::string portStr = parseUrl(url, host, path);
    int port = std::stoi(portStr);
    
    if (!connectToHost(host, port)) {
        throw std::runtime_error("Failed to connect to host");
    }
    
    std::string request = buildHttpRequest("DELETE", path, headers);
    Write(request.c_str());
    
    return readHttpResponse();
} 