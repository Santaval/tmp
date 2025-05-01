#include "Client.hpp"
#include <iostream>
#include "Holder.hpp"
#include "Server.hpp"

int main(){
  int error = EXIT_SUCCESS;
  // init shared data
  Buffer* client_holder_buffer = new Buffer();
  Buffer* holder_server_buffer = new Buffer();
  sem_t* client_request_sem = new sem_t;
  sem_t* client_response_sem = new sem_t;
  sem_t* mktp_request_sem = new sem_t;
  sem_t* mktp_response_sem = new sem_t; 

  // init semaphores

  error = sem_init(client_request_sem, /*pshared*/ 0, /*value*/ 0);
  error = sem_init(client_response_sem, /*pshared*/ 0, /*value*/ 0);
  error = sem_init(mktp_request_sem, /*pshared*/ 0, /*value*/ 0);
  error = sem_init(mktp_response_sem, /*pshared*/ 0, /*value*/ 0);


  Client client(client_holder_buffer, client_request_sem, client_response_sem);
  Server server(holder_server_buffer, mktp_request_sem, mktp_response_sem);
  Holder holder(client_holder_buffer, holder_server_buffer, client_request_sem, client_response_sem, mktp_request_sem, mktp_response_sem);

  // start threads
  if (client.start() == EXIT_FAILURE) printf("c failure\n");
  if (holder.start() == EXIT_FAILURE) printf("h failure\n");
  if (server.start() == EXIT_FAILURE) printf("s failure\n");
  //holder.start();
  //server.start();


  // join threads
  client.join();
  server.join();
  holder.join();

  // destroy semaphores
  error = sem_destroy(client_request_sem);
  error = sem_destroy(client_response_sem);
  error = sem_destroy(mktp_request_sem);
  error = sem_destroy(mktp_response_sem);
  

  // destroy shared data
  delete client_holder_buffer;
  delete holder_server_buffer;
  
  return error;
}