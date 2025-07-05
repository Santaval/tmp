#ifndef HTTPPARSER_H
#define HTTPPARSER_H

#include <string>
#include <map>

class HttpParser {
public:
    struct HttpRequest {
        std::string method;
        std::string path;
        std::string version;
        std::map<std::string, std::string> headers;
        std::string body;
    };

    // Parses a raw HTTP request string into an HttpRequest struct
    static HttpRequest parseRequest(const std::string& rawRequest);

    // Utility: parse only the request line (method, path, version)
    static void parseRequestLine(const std::string& requestLine, std::string& method, std::string& path, std::string& version);

    // Utility: parse headers from a block of header lines
    static std::map<std::string, std::string> parseHeaders(const std::string& headersBlock);

    // Utility: create a simple HTTP response message with correct headers for browser
    static std::string createHttpResponse(const std::string& content);
};

#endif // HTTPPARSER_H