#pragma once
#include <functional>
#include <string>
#include "http/HttpParser.h"
#include "sockets/includes/VSocket.hpp"

class ForkRoutes {
public:
    static void registerRoutes(class Fork* fork);
};
