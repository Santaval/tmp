#include "Client.hpp"
#include <iostream>
#include <ostream>
#include <string>

// should come with an arg for testing
int Client::run() {
    while (true) {
        std::string input;
        std::cout << "Enter HTTP command: ";
        std::cin >> input;
        this->sendHTTP(input.c_str());
        sem_post(this->client_request_sem);
        sem_wait(this->client_response_sem);
        this->manageResponse();
    }
    
    return 0;
}

int Client::sendHTTP(const char* request) {
    this->buffer->write("GET", request);
}

int Client::manageResponse() {
    if (this->buffer->contains("GET")) {
        std::cout << this->buffer->read("GET") << std::endl;
    } else {
        std::cout << "Client::manageResponse - Invalid response" << std::endl;
        return -1;
    }
}