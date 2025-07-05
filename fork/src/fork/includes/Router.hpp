#pragma once
#include <functional>
#include <map>
#include <string>
#include <vector>

class Router {
public:
    using Handler = std::function<void(const std::string& body, void* socket, const std::map<std::string, std::string>& params)>;
    void addRoute(const std::string& method, const std::string& path, Handler handler);
    bool handle(const std::string& method, const std::string& path, const std::string& body, void* socket);
private:
    struct ParamRoute {
        std::string method;
        std::string path;
        Handler handler;
    };
    std::map<std::string, Handler> routes;
    std::vector<ParamRoute> paramRoutes;
    std::string makeKey(const std::string& method, const std::string& path) const;
};
