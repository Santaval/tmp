#ifndef Client_hpp
#define Client_hpp

#include "Socket.hpp"
#include <string>
#include <map>
#include "pig/includes/Message.h"

/**
 * @brief HTTP Client class for making HTTP requests
 */
class Client : public Socket {
public:
    Client();
    ~Client();

    // HTTP Methods
    std::string send(const std::string& url, std::string& message, 
                     MessageType type = MessageType::ERROR);

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