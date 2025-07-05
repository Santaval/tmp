#include "Router.hpp"
#include <regex>
#include <map>

void Router::addRoute(const std::string& method, const std::string& path, Handler handler) {
    routes[makeKey(method, path)] = handler;
    // Store parameterized route patterns
    std::regex paramRegex(":([a-zA-Z0-9_]+)");
    if (std::regex_search(path, paramRegex)) {
        paramRoutes.push_back({method, path, handler});
    }
}

bool Router::handle(const std::string& method, const std::string& path, const std::string& body, void* socket) {
    auto it = routes.find(makeKey(method, path));
    if (it != routes.end()) {
        it->second(body, socket, {}); // No params
        return true;
    }
    // Check parameterized routes
    for (const auto& route : paramRoutes) {
        if (route.method != method) continue;
        std::smatch match;
        std::string pattern = route.path;
        // Convert :param to regex group
        pattern = std::regex_replace(pattern, std::regex(R"(:([a-zA-Z0-9_]+))"), R"(([^/]+))");
        pattern = "^" + pattern + "$";
        std::regex re(pattern);
        if (std::regex_match(path, match, re)) {
            // Extract param names
            std::map<std::string, std::string> params;
            std::regex paramNameRe(":([a-zA-Z0-9_]+)");
            auto names_begin = std::sregex_iterator(route.path.begin(), route.path.end(), paramNameRe);
            auto names_end = std::sregex_iterator();
            int i = 1;
            for (auto itn = names_begin; itn != names_end; ++itn, ++i) {
                params[itn->str(1)] = match[i];
            }
            route.handler(body, socket, params);
            return true;
        }
    }
    return false;
}

std::string Router::makeKey(const std::string& method, const std::string& path) const {
    return method + ":" + path;
}
