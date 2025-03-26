#include "Client.hpp"
#include <iostream>


int Client::run() {
    std::cout << "Client ----- Sending request" << std::endl;

    sem_post(this->client_request_sem);
    std::cout << "Client ----- Waiting for response" << std::endl;
    sem_wait(this->client_response_sem);
    std::cout << "Client ----- Received response" << std::endl;
    return 0;
}