#include "Holder.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "utils.hpp"

int Holder::run() {
    while (true) {
        sem_wait(client_request_sem);
        std::cout << "[-] Holder: recieved request from Client: ";
        if (this->manageClientRequest() != -1) {
            sem_post(mktp_request_sem);
            std::cout << "[-] Holder: request sent to Server" << std::endl;
            sem_wait(mktp_response_sem);
            std::cout << "[-] Holder: recieved response from Server" << std::endl;
            if (this->manageServerResponse() == -1) {
                return -1;
            }
            this->answerHTTP();           
        }
        std::cout << "[-] Holder: response sent to Client" << std::endl;
        sem_post(client_response_sem); 
    }
    return 0;
    
}
int Holder::manageClientRequest() {
    if (!this->client_holder_buffer->contains("GET")) {
        std::cout << "[-] Holder::manageRequest - Invalid http method"<< std::endl;
        this->client_holder_buffer->write("GET", "Bad Request");
        return -1;
    }
    std::string content = this->client_holder_buffer->read("GET");
    if(content.length() < 2) { // an empty request was sent
        std::cout << "[-] Holder::manageRequest - Not directory specified"<< std::endl;
        this->client_holder_buffer->write("GET", "Bad Request");
        return -1;
    }
    std::cout << content << std::endl;
    this->sendMKTP(content);
    return 0;
}
int Holder::sendMKTP(std::string content) {
    std::vector <std::string> tokens = splitValue(content, '/');
    if(tokens.size() >= 3) { // a specific file was attached
        this->holder_server_buffer->write("ONE", tokens[2]);
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
    response = this->holder_server_buffer->read(key);
    tokens = splitValue(response, ' ');

    if (tokens[0] == "0") {
        if (tokens[1]== "NF") response = "[*] File Not Found";
        if (tokens[1]== "SE") response = "[*] Server Error";
        if (tokens[1]== "BR") response = "[*] Bad Request";
        
    } else if (tokens[0] == "1") {
        response = tokens[1];
    }
    else {
        std::cout << "[-] Holder: Unexpected server answer\n";
        return -1;
    }
    this->client_holder_buffer->write("GET", response);
    return 0;
}
