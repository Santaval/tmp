#include "HttpParser.h"
#include <sstream>
#include <algorithm>
#include <iostream>

HttpParser::HttpRequest HttpParser::parseRequest(const std::string& rawRequest) {
    HttpRequest req;
    size_t lineEnd = rawRequest.find("\r\n");
    if (lineEnd == std::string::npos) return req;
    std::string requestLine = rawRequest.substr(0, lineEnd);
    parseRequestLine(requestLine, req.method, req.path, req.version);

    // Find headers block
    size_t headersStart = lineEnd + 2;
    size_t headersEnd = rawRequest.find("\r\n\r\n", headersStart);
    if (headersEnd == std::string::npos) return req;
    std::string headersBlock = rawRequest.substr(headersStart, headersEnd - headersStart);
    req.headers = parseHeaders(headersBlock);

    // Body (if any)
    size_t bodyStart = headersEnd + 4;
    if (bodyStart < rawRequest.size()) {
        req.body = rawRequest.substr(bodyStart);
    }
    return req;
}

void HttpParser::parseRequestLine(const std::string& requestLine, std::string& method, std::string& path, std::string& version) {
    std::istringstream iss(requestLine);
    iss >> method >> path >> version;
}

std::map<std::string, std::string> HttpParser::parseHeaders(const std::string& headersBlock) {
    std::map<std::string, std::string> headers;
    std::istringstream stream(headersBlock);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        size_t colon = line.find(":");
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            // Trim whitespace
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            headers[key] = value;
        }
    }
    return headers;
}

std::string HttpParser::createHttpResponse(const std::string& content) {
    std::string response = content;
    int statusCode = 200; // Default to OK
    std::string contentType = "text/html"; // Default content type
   	std::string httpResponse = "HTTP/1.1 " + std::to_string(statusCode) + " " + 
							 (statusCode == 200 ? "OK" : "Not Found") + "\r\n";
	httpResponse += "Content-Type: " + contentType + "; charset=UTF-8\r\n";
	httpResponse += "Content-Length: " + std::to_string(response.length()) + "\r\n";
	httpResponse += "Connection: close\r\n";
	httpResponse += "\r\n";
	httpResponse += response + "\r\n\r\n";
    return httpResponse;
}