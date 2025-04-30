#include "Server.hpp"
#include <iostream>
#include <fstream>
#include <regex>
#include <thread>

int Server::run()
{
    bool connected = true;

    std::thread  discoveryThread(&Server::startDiscovery, this, discovery_buffer);
    discoveryThread.detach();

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

Server::~Server() {
    discovery_buffer->write("Discovery", "BEGIN/OFF/SERVIDOR/END");
}

int Server::answerMKTP() {
    std::string raw = this->holder_server_buffer->read("SERVER_REQUEST");
    std::string response;

    std::smatch match;
    std::regex getRegex(R"(BEGIN/GET/([^/]+)/END)");

    if (raw == "BEGIN/OBJECTS/END") {
        // Formato compatible con holder
        response = "BEGIN/OK/asthetic.txt\nchillGuy.txt\nlakshmi.txt\nMichaelMouse.txt\nvalorant.txt/END";
    } 
    else if (std::regex_match(raw, match, getRegex)) {
        std::string filename = match[1];
        std::ifstream file("./resources/" + filename);

        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), 
                            std::istreambuf_iterator<char>());
            file.close();
            response = "BEGIN/OK/" + content + "/END";
        } else {
            response = "BEGIN/ERROR/201/Archivo no encontrado/END";
        }
    }
    else {
        response = "BEGIN/ERROR/300/Formato de mensaje inválido/END";
    }

    this->holder_server_buffer->write("SERVER_RESPONSE", response);
    return 0;
}

void Server::startDiscovery(Buffer* discovery_buffer) {
    while (true) {
        discovery_buffer->write("Discovery", "BEGIN/ON/SERVIDOR/END");
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}
