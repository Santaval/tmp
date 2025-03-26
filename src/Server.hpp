
#pragma once
#include "Thread.hpp"
#include "Buffer.hpp"
#include <semaphore.h>

class Server: public Thread {
  private:
    Buffer* holder_server_buffer;
    sem_t* mktp_request_sem;
    sem_t* mktp_response_sem;
  public: 
    Server(Buffer* holder_server_buffer, sem_t* mktp_request_sem, sem_t* mktp_response_sem) :
      holder_server_buffer(holder_server_buffer), mktp_request_sem(mktp_request_sem), mktp_response_sem(mktp_response_sem) {};
    ~Server() = default;
    int run() override;
};