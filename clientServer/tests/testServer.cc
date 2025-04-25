#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

//#include "../VServer.hh"
#include "../Server.hh"
int main(void) {
  VServer* server;
	// 5052 is the port in which server is listening for clients (clients must connect through same port)
	server = new Server(5052, "127.0.0.1");
	// start server with its same ip (unused for now)
	server->start("127.0.0.1"); 
}