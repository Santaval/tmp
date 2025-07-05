# HttpParser Usage Guide

The `HttpParser` class provides static methods to parse raw HTTP request strings into structured data. This is useful for handling HTTP requests in your server code.

## Features
- Parse the HTTP request line (method, path, version)
- Parse HTTP headers into a map
- Extract the request body

## API

### Struct: `HttpParser::HttpRequest`
```cpp
struct HttpRequest {
    std::string method;   // e.g., "GET"
    std::string path;     // e.g., "/index.html"
    std::string version;  // e.g., "HTTP/1.1"
    std::map<std::string, std::string> headers; // header key-value pairs
    std::string body;     // request body (if any)
};
```

### Method: `static HttpRequest parseRequest(const std::string& rawRequest)`
Parses a full HTTP request string into an `HttpRequest` struct.

### Method: `static void parseRequestLine(const std::string& requestLine, std::string& method, std::string& path, std::string& version)`
Parses the request line (first line) into its components.

### Method: `static std::map<std::string, std::string> parseHeaders(const std::string& headersBlock)`
Parses a block of header lines into a map.

## Example Usage

```cpp
#include "HttpParser.h"
#include <iostream>

int main() {
    std::string rawRequest =
        "GET /hello.txt HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "User-Agent: curl/7.68.0\r\n"
        "Accept: */*\r\n"
        "\r\n"
        "body content here";

    HttpParser::HttpRequest req = HttpParser::parseRequest(rawRequest);

    std::cout << "Method: " << req.method << std::endl;
    std::cout << "Path: " << req.path << std::endl;
    std::cout << "Version: " << req.version << std::endl;
    for (const auto& h : req.headers) {
        std::cout << "Header: " << h.first << " => " << h.second << std::endl;
    }
    std::cout << "Body: " << req.body << std::endl;
}
```

## Notes
- The parser expects CRLF (`\r\n`) line endings as per HTTP/1.1 standard.
- If the request is malformed, the returned struct fields may be empty.
- Only request parsing is supported (not response parsing). 