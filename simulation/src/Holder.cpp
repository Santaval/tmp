#include "Holder.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "utils.hpp"
#include <regex>

int Holder::run() {
    bool connected = true;
    while (connected) {
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
    std::string raw = this->client_holder_buffer->read();

    std::smatch match;
    std::regex getRegex(R"(BEGIN/GET/([^/]+)/END)");

    if (!std::regex_match(raw, match, getRegex)) {
        std::cout << "[-] Holder::manageclientRequest - Formato invalido" << std::endl;
        this->client_holder_buffer->write("BEGIN/ERROR/300/Formato de mensaje inválido/END");
        return -1;
    }

    std::string resource = match[1];
    if (resource.empty()) {
        std::cout << "[-] Holder::manageclientRequest - Recurso vacio" << std::endl;
        this->client_holder_buffer->write("BEGIN/ERROR/200/Recurso no especificado/END");
        return -1;
    }

    return this->sendMKTP(resource);
   // return 0;
}

int Holder::sendMKTP(std::string resource) {
    if (resource == "ALL") {
        this->holder_server_buffer->write("BEGIN/OBJECTS/END");
    } else {
        this->holder_server_buffer->write("BEGIN/GET/" + resource + "/END");
    }

    return 0;
}

int Holder::manageServerResponse() {
    std::string raw = this->holder_server_buffer->read();

    if (raw.rfind("BEGIN/OK/", 0) == 0) {
        std:: string data = raw.substr(9, raw.size() - 13);
        this->lastServerCode = 1;
        this->lastServerMessage = data;
        return 0;
    }

    std::smatch match;
    std::regex errorRegex(R"(BEGIN/ERROR/(\d{3})/([^/]+)/END)");

    if (std::regex_match(raw, match, errorRegex)) {
        std::string errorCode = match[1];
        std::string errorMessage = match[2];
        this->lastServerCode = std::stoi(errorCode);
        this->lastServerMessage = errorMessage;
        return 0;
    }

    std::cout << "[-] Holder::manageServerResponse - Respuesta inesperada del servidor" << std::endl;
    return -1;
}

int Holder::answerHTTP() {
    if (lastServerCode == 1) {
        this->client_holder_buffer->write("BEGIN/OK/" + lastServerMessage + "/END");
    } else {
        this->client_holder_buffer->write("BEGIN/ERROR/" + std::to_string(lastServerCode) + "/" + lastServerMessage + "/END");
    }
    return 0;

}

void Holder::listenToDiscovery(Buffer* discovery_buffer) {
    while (true) {
        std::string msg = discovery_buffer->read("DISCOVERY");
        if (msg == "BEGIN/ON/SERVIDOR/END") {
            serverAvailable = true;
        } else if (msg == "BEGIN/OFF/SERVIDOR/END") {
            serverAvailable = false;
        }
    }
}
