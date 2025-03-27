#include "Server.hpp"
#include <iostream>
#include <fstream>

int Server::run()
{
    while (true)
    {   
        std::cout << "Server is waiting" << std::endl;

        sem_wait(mktp_request_sem);
        this->answerMKTP();
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
        response = "1 asthetic.txt\nchillGuy.txt\nlakshmi.txt\nMichaelMouse.txt\nvalorant.txt\n";
    }

    this->holder_server_buffer->write(key, response);

    return 0;
}
