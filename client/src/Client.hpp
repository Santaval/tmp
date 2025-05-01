#ifndef Client_hpp
#define Client_hpp

#include "Socket.hpp"
#include <string>
#include <map>

/**
 * @brief HTTP Client class for making HTTP requests
 */
class Client : public Socket {
public:
    Client();
    ~Client();

    // HTTP Methods
    std::string GET(const std::string& url, const std::map<std::string, std::string>& headers = {});
    std::string POST(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers = {});
    std::string PUT(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers = {});
    std::string DELETE(const std::string& url, const std::map<std::string, std::string>& headers = {});

private:
    // Helper methods
    std::string parseUrl(const std::string& url, std::string& host, std::string& path);
    std::string buildHttpRequest(const std::string& method, const std::string& path, 
                               const std::map<std::string, std::string>& headers,
                               const std::string& body = "");
    std::string readHttpResponse();
    bool connectToHost(const std::string& host, int port = 80);
};

#endif // Client_hpp