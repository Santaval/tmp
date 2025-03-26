#include "Client.hpp"
#include <iostream>


int Client::run() {
    std::cout << "Client thread started" << std::endl;
    std::cout << "Writing to buffer" << std::endl;

    sem_post(this->client_request_sem);
    std::cout << "Waiting for response" << std::endl;
    sem_wait(this->client_response_sem);
    return 0;
}