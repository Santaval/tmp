#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../Client.hh"

int main(void) {
   // init client with Client ip and port in which server is listening
   Client* client = new Client(5052, "127.0.0.1");
   // start client with Server ip
   client->start("127.0.0.1");
   return 0;
}