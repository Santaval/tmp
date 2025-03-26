#include "Server.hpp"
#include <iostream>

int Server::run() {
    std::cout << "Server ----- waiting for holder request" << std::endl;
    sem_wait(mktp_request_sem);
    std::cout << "Server ----- received holder request" << std::endl;
    sem_post(mktp_response_sem);
    std::cout << "Server ----- sent response to holder" << std::endl;
    return 0;
}