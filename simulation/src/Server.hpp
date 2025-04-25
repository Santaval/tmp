
#pragma once
#include "Thread.hpp"
#include "Buffer.hpp"
#include <semaphore.h>

class Server: public Thread {
  private:
    Buffer* holder_server_buffer;
    sem_t* mktp_request_sem;
    sem_t* mktp_response_sem;
    std::string image;
    std::string image_2;
    std::string image_3;
    std::string image_4;
    std::string image_5;
  public: 
    Server(Buffer* holder_server_buffer, sem_t* mktp_request_sem, sem_t* mktp_response_sem) :
      holder_server_buffer(holder_server_buffer), mktp_request_sem(mktp_request_sem), mktp_response_sem(mktp_response_sem) {};
    ~Server() = default;
    int run() override;
    int answerMKTP();
};