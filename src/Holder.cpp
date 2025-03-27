#include "Holder.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>

int Holder::run() {
    while (true) {
        sem_wait(client_request_sem);
    
        if (this->manageClientRequest() == -1) {
            return -1;
        }
        this->sendMKTP();
        sem_post(mktp_request_sem);
        sem_wait(mktp_response_sem);
        
        if (this->manageServerResponse() == -1) {
            return -1;
        }
        this->answerHTTP();
        sem_post(client_response_sem);
        return 0;
    }
    
}
int Holder::manageClientRequest() {
    std::cout << "[+] Client: "<< this->client_holder_buffer->read("GET") << std::endl;
    if (!this->client_holder_buffer->contains("GET")) {
        std::cout << "Holder::manageRequest - Invalid http method"<< std::endl;
        return -1;
    }
    std::string content = this->client_holder_buffer->read("GET");
    if(content.length() < 2) { // an empty request was sent
         std::cout << "Holder::manageRequest - Not directory specified"<< std::endl;
        return -1;
    }
    return 0;
}
int Holder::sendMKTP() {
    std::string content = this->client_holder_buffer->read("GET");
    std::cout << "Content: " << content << std::endl;
    std::vector <std::string> tokens = this->client_holder_buffer->splitValue("GET", '/');
    if(tokens.size() >= 2) { // a specific file was attached
        this->holder_server_buffer->write("ONE", content);
    } else { // asked for directory
        this->holder_server_buffer->write("ALL", content);
    }
    return 0;
}
int Holder::manageServerResponse() {
    std::string key;
    if (this->holder_server_buffer->contains("ALL") ||
     this->holder_server_buffer->contains("ONE")) {
        return 0;
    } else {
        return -1;
    }
    
}
int Holder::answerHTTP() {
    std::string response;
    std::vector <std::string> tokens;
    std::string key = "ALL";
    if (this->holder_server_buffer->contains("ONE")) {
        key = "ONE";
    }
    tokens = this->holder_server_buffer->splitValue(key, ' '); // ALL 0 NF --- ALL 1 [content]
    if (tokens[0] == "0") {
        if (tokens[1]== "NF") response = "File Not Found";
        if (tokens[1]== "SE") response = "Server Error";
        if (tokens[1]== "BR") response = "Bad Request";
        
    } else if (tokens[0] == "1") {
        response = this->holder_server_buffer->read(key);
    }
    else {
        std::cout << "Holder::answerHTTP -- Unexpected server answer\n";
        return -1;
    }
    this->client_holder_buffer->write("GET", response);
    return 0;
}
