#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

typedef struct shared_data {
  std::string buffer;

  sem_t* holder_sem_start;
  sem_t* server_sem_start;

  sem_t* client_request_sem; 
  sem_t* client_response_sem;
  sem_t* mktp_request_sem; 
  sem_t* mktp_response_sem; 

} shared_data_t;

typedef struct private_data {
  shared_data_t* shared_data;
} private_data_t;


void * client(void * data){
  return NULL;
}

void * server(void * data){
  return NULL;
}

void * holder(void * data){
  return NULL; 
}

int main(){
  int error = EXIT_SUCCESS;

  shared_data_t* shared_data = (shared_data_t*)calloc(1, sizeof(shared_data_t));
  pthread_t* threads = (pthread_t*) malloc(3 * sizeof(pthread_t));
  private_data_t* private_data = (private_data_t*) calloc(3, sizeof(private_data_t));

  if (shared_data) {
    error = sem_init(shared_data->server_sem_start, /*pshared*/ 0, /*value*/ 0);
    error = sem_init(shared_data->holder_sem_start, /*pshared*/ 0, /*value*/ 0);
    error = sem_init(shared_data->mktp_request_sem, /*pshared*/ 0, /*value*/ 0);
    error = sem_init(shared_data->mktp_response_sem, /*pshared*/ 0, /*value*/ 0);
    error = sem_init(shared_data->client_request_sem, /*pshared*/ 0, /*value*/ 0);
    error = sem_init(shared_data->client_response_sem, /*pshared*/ 0, /*value*/ 0);
  }

  error = pthread_create(&threads[0], /*attr*/ NULL, client, /*arg*/ &private_data[0]);
  error = pthread_create(&threads[1], /*attr*/ NULL, server, /*arg*/ &private_data[1]);
  error = pthread_create(&threads[2], /*attr*/ NULL, holder, /*arg*/ &private_data[2]);

  for (uint64_t thread_number = 0; thread_number < 3; ++thread_number) {
    pthread_join(threads[thread_number], /*value_ptr*/ NULL);
  }

  sem_destroy(shared_data->server_sem_start);
  sem_destroy(shared_data->holder_sem_start);
  sem_destroy(shared_data->mktp_request_sem);
  sem_destroy(shared_data->mktp_response_sem);
  sem_destroy(shared_data->client_request_sem);
  sem_destroy(shared_data->client_response_sem);
}