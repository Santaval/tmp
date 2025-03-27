#include "Server.hpp"
#include <iostream>

int Server::run() {
    sem_wait(mktp_request_sem);


    this->answerMKTP();
    sem_post(mktp_response_sem);
    return 0;
}

int Server::answerMKTP() {
    std::string response;
    std::string key = "ALL";

    if (this->holder_server_buffer->contains("ONE")) {
        key = "ONE";
        std::string file_requested = this->holder_server_buffer->read("ONE");

        if ( file_requested == "MichaelMouse.txt") {
            response = this->image;
        } else if (file_requested == "chillGuy.txt") {
            response = this->image_2;
        } else if (file_requested == "valorant.txt") {
            response = this->image_3;
        } else if (file_requested == "lakshmi.txt") {
            response = this->image_4;
        } else if (file_requested == "aesthetic.txt") {
            response = this->image_5;
        } else {
            response = "NF";
        }
    } else {
        response = this->image + this->image_2 + this->image_3 + this->image_4 + this->image_5;
    }

    this->holder_server_buffer->write(key, response);
    return 0;
}
