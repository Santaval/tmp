#include "Holder.hpp"
#include <iostream>

int Holder::run() {
    std::cout << "Holder ----- waiting for client request" << std::endl;
    sem_wait(client_request_sem);
    std::cout << "Holder ----- received client request" << std::endl;

    std::cout << "Holder ----- Send request to server" << std::endl;
    sem_post(mktp_request_sem);
    std::cout << "Holder ----- waiting for response from server" << std::endl;
    sem_wait(mktp_response_sem);
    std::cout << "Holder ----- received response from server" << std::endl;

    sem_post(client_response_sem);
    std::cout << "Holder ----- sent response to client" << std::endl;
    return 0;
}