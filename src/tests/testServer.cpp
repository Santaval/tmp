#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../VServer.hpp"
#include "../Server.hpp"
int main(void) {
  VServer* server;
	/* since its local, the sending and listening ports willb be reversed between Holder-Server, meaning
	5051 = through which Server first attempts to send a msg to holder
	8081 = in which Server stays listening for a msg from Holder
	
	*/
	server = new Server(5051, 8081, "127.0.0.1");
	server->start("127.0.0.1");
}