#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../VServer.hpp"
#include "../Holder.hpp"
int main(void) {
  VServer* holder;
	/* since its local, the sending and listening ports willb be reversed between Holder-Server, meaning
	8081 = through which Holder first attempts to send a msg to Server
	5051 = in which Holder stays listening for a msg from server
	Later on, 5052 is thw port in which Holder stays listening for clients
	*/
	holder = new Holder(8081, 5051, 5052, "127.0.0.1");
	
	holder->start("127.0.0.1");
}