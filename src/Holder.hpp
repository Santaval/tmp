
#pragma once
#include "Thread.hpp"
#include "Buffer.hpp"
#include <semaphore.h>

class Holder: public Thread {
  private:
    Buffer* client_holder_buffer;
    Buffer* holder_server_buffer;
    sem_t* client_request_sem;
    sem_t* client_response_sem;
    sem_t* mktp_request_sem;
    sem_t* mktp_response_sem;
  public: 
    Holder(Buffer* client_holder_buffer, Buffer* holder_server_buffer, sem_t* client_request_sem, sem_t* client_response_sem, sem_t* mktp_request_sem, sem_t* mktp_response_sem) :
      client_holder_buffer(client_holder_buffer),
      holder_server_buffer(holder_server_buffer),
      client_request_sem(client_request_sem),
      client_response_sem(client_response_sem),
      mktp_request_sem(mktp_request_sem),
      mktp_response_sem(mktp_response_sem) 
      {}
    ~Holder() = default;
    int run() override;
    int manageClientRequest();
    int manageServerResponse();
    int sendMKTP();
    int answerHTTP();
};