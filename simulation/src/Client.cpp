#include "Client.hpp"
#include <iostream>
#include <ostream>
#include <string>
#include <unistd.h>

// should come with an arg for testing
int Client::run() {
    bool connected = true;
    while (connected) {
        std::string input = "/resources";
        std::cout << "[*] Enter HTTP command: ";
        std::cin >> input;
        std::string request = "BEGIN/GET/" + input + "/END";
        this->sendHTTP(request.c_str());
        sem_post(this->client_request_sem);
        sem_wait(this->client_response_sem);
        //this->manageResponse();
        if (this->manageResponse() == -1) {
            std::cout << "[*] Client: Error in response" << std::endl;
        } else {
            std::cout << "[*] Client: Response received" << std::endl;
        }
    }
    return 0;
}

int Client::sendHTTP(const char* request) {
    this->buffer->write("CLIENT_REQUEST", request);
    return 0;
}

int Client::manageResponse() {

    if (!this->buffer->contains("CLIENT_RESPONSE")) {
        std::cout << "[*] Client: No response from server" << std::endl;
        return -1;
    }

    std::string response = this->buffer->read("CLIENT_RESPONSE");

    if (response.rfind("BEGIN/OK/", 0) == 0) {
        std::string content = response.substr(9, response.size() - 13);
        std::cout << "[*] Client: Response OK \n" << content << std::endl;
        
    } else if (response.rfind("BEGIN/ERROR/", 0) == 0) {
       size_t codePos = 12;
       size_t message_Pos = response.find("/", codePos);
       std::string code = response.substr(codePos, message_Pos - codePos);
       std::string message = response.substr(message_Pos + 1, response.size() - message_Pos - 5);

       std::cout << "[*] Client: Error " << code << ": " << message << std::endl;
       return -1;
   
    } else {
        std::cout << "[*] Invalid response" << std::endl;
        return -1;
    }
    return 0;
}