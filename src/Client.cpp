#include "Client.hpp"
#include <iostream>

// should come with an arg for testing
int Client::run() {
    std::cout << "Client ----- Sending request" << std::endl;
    this->sendHTTP("/resources/MichaelMouse.txt");
    sem_post(this->client_request_sem);
    std::cout << "Client ----- Waiting for response" << std::endl;
    sem_wait(this->client_response_sem);
    std::cout << "Client ----- Received response" << std::endl;
    
    return 0;
}

int Client::sendHTTP(const char* request) {
    this->buffer->checkAndSet("GET", "", request);
}

int Client::manageResponse() {
    if (this->buffer->contains("GET")) {
        std::cout << this->buffer->read("GET") << std::endl;
    }
}