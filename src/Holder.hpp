
#pragma once
#include "Thread.hpp"
#include "Buffer.hpp"
#include <semaphore.h>

/// @brief A holder to act as an intermediary between the client and the server
class Holder: public Thread {
  private:
    /// @brief Buffer to communicate with the client
    Buffer* client_holder_buffer;
    /// @brief Buffer to communicate with the server
    Buffer* holder_server_buffer;
    /// @brief Semaphore to wait for receiving a request
    sem_t* client_request_sem;
    /// @brief Semaphore to wait for sending a response
    sem_t* client_response_sem;
    /// @brief Semaphore to wait for sending  a request
    sem_t* mktp_request_sem;
    /// @brief Semaphore to wait for receiving a response
    sem_t* mktp_response_sem;
  public:
    /// @brief Constructor
    Holder(Buffer* client_holder_buffer, Buffer* holder_server_buffer, sem_t* client_request_sem, sem_t* client_response_sem, sem_t* mktp_request_sem, sem_t* mktp_response_sem) :
      client_holder_buffer(client_holder_buffer),
      holder_server_buffer(holder_server_buffer),
      client_request_sem(client_request_sem),
      client_response_sem(client_response_sem),
      mktp_request_sem(mktp_request_sem),
      mktp_response_sem(mktp_response_sem)
      {}
    /// @brief Destructor
    ~Holder() = default;
    /// @brief Execute thread
    int run() override;
    /// @brief Receives the client's request and, if valid, sends it to the server.
    int manageClientRequest();
    /// @brief Receives the server's response and sends the answer to the client.
    int manageServerResponse();
    /// @brief Sends to the server the request in the MKTP protocol
    int sendMKTP();
    /// @brief Sends to the client the response in the HTTP protocol
    int answerHTTP();
};