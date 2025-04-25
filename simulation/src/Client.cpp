#include "Client.hpp"
#include <iostream>
#include <ostream>
#include <string>
#include <unistd.h>

// should come with an arg for testing
int Client::run() {
    bool connected = true;
    while (connected) {
        std::string input;
        std::cout << "[*] Enter HTTP command: ";
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
    return 0;
}

int Client::manageResponse() {
    if (this->buffer->contains("GET")) {
        sleep(0.58);
        std::cout << this->buffer->read("GET") << std::endl;
        
    } else {
        std::cout << "[*] Invalid response" << std::endl;
        return -1;
    }
    return 0;
}