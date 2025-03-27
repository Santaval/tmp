#include "Server.hpp"
#include <iostream>
#include <fstream>

int Server::run()
{
    bool connected = true;
    while (connected)
    {
        sem_wait(mktp_request_sem);
        std::cout << "[+] Server: recieved request from Holder: ";
        if (this->answerMKTP() == -1) connected = false;
        std::cout << "[+] Server: response sent to Holder" << std::endl;
        sem_post(mktp_response_sem);
    }
    return 0;
}

int Server::answerMKTP()
{
    std::string response;
    std::string key = "ALL";
    if (this->holder_server_buffer->contains("ONE"))
    {
        key = "ONE";
        std::string file_requested = this->holder_server_buffer->read("ONE");
        std::cout << file_requested << std::endl;
        std::ifstream file;
        file.open("./resources/" + file_requested);
        if (file.is_open())
        {
            std::string line;
            response += "1 ";
            while (getline(file, line))
            {
                response += line + "\n";
            }
            file.close();
        }
        else
        {
            response = "0 NF";
        }
    }
    else
    {   
        std::cout << std::endl;
        response = "1 --resources\n---asthetic.txt\n---chillGuy.txt\n---lakshmi.txt\n---MichaelMouse.txt\n---valorant.txt\n";
    }

    this->holder_server_buffer->write(key, response);

    return 0;
}
