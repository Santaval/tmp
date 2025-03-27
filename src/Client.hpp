
#pragma once
#include "Thread.hpp"
#include "Buffer.hpp"
#include <semaphore.h>

class Client: public Thread {
  private:
    Buffer* buffer;
    sem_t* client_request_sem;
    sem_t* client_response_sem;
  public: 
    Client(Buffer* buffer, sem_t* client_request_sem, sem_t* client_response_sem) :
      buffer(buffer),
      client_request_sem(client_request_sem),
      client_response_sem(client_response_sem)
      {}
    ~Client() = default;
    int run() override;
    int sendHTTP(const char*);
    int manageResponse();
    
};