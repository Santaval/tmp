
#pragma once
#include "Thread.hpp"
#include "Buffer.hpp"
#include <semaphore.h>

class Client: public Thread {
  private:
    /// @brief  Shared buffer with the holder to send messages.
    Buffer* buffer;
    /// @brief Semaphore to send request
    sem_t* client_request_sem;
    /// @brief Semaphore to wait for a response.
    sem_t* client_response_sem;
  public:
    /// @brief Constructor
    Client(Buffer* buffer, sem_t* client_request_sem, sem_t* client_response_sem) :
      buffer(buffer),
      client_request_sem(client_request_sem),
      client_response_sem(client_response_sem)
      {}
    /// @brief Destructor
    ~Client() = default;
    /// @brief Execute thread
    /// @return Returns state value. O if success
    int run() override;
    /// @brief Sends the HTTP request to the holder
    /// @param request The request to be make to the serer. Written in HTTP
    /// @return Returns state value. O if success
    int sendHTTP(const char*);
    /// @brief Receives and prints the response from the server.
    /// @return Returns state value. O if success
    int manageResponse();

};