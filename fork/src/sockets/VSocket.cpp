/**
*  Universidad de Costa Rica
*  ECCI
*  CI0123 Proyecto integrador de redes y sistemas operativos
*  2025-i
*  Grupos: 1 y 3
*
****** VSocket base class implementation
*
* (Fedora version)
*
**/

#include <sys/socket.h>
#include <arpa/inet.h>		// ntohs, htons
#include <stdexcept>            // runtime_error
#include <cstring>		// memset
#include <netdb.h>			// getaddrinfo, freeaddrinfo
#include <unistd.h>			// close
#include <iostream>
/*
#include <cstddef>
#include <cstdio>

//#include <sys/types.h>
*/
#include "includes/VSocket.hpp"


/**
 *  Class creator (constructor)
 *     use Unix socket system call
 *
 *  @param     char t: socket type to define
 *     's' for stream
 *     'd' for datagram
 *  @param     bool ipv6: if we need a IPv6 socket
 *
**/
void VSocket::BuildSocket( char t, bool IPv6 ){
  int st = -1;
  if (t == 's' || t == 'd') {
    st = (t == 's')? SOCK_STREAM : SOCK_DGRAM;
  }

  if ( -1 == st ) {
    throw std::runtime_error("VSocket::BuildSocket, No valid socket typeeee");
  }
  this->type = t;
  this->IPv6 = IPv6;
  int domain = IPv6 ? PF_INET6 : AF_INET;
  this->idSocket = socket(domain, st, 0);
  
  if (this->idSocket == -1) {
    throw std::runtime_error("VSocket::BuildSocket, Unable to create socket");
  }


}


/**
 * Class destructor
 *
**/
VSocket::~VSocket() {

  this->Close();
  this->Shutdown(SHUT_RDWR);
}


/**
 * Close method
 *    use Unix close system call (once opened a socket is managed like a file in Unix)
 *
**/
void VSocket::Close(){
  int st = -1;
  st = close(this->idSocket); // close the open socket, ret 0 if successful
  if ( -1 == st ) {
      throw std::runtime_error( "VSocket::Close()" );
  }

}


/**
 * EstablishConnection method
 *   use "connect" Unix system call
 *
 * @param      char * host: host address in dot notation, example "10.84.166.62"
 * @param      int port: process address, example 80
 *
**/
int VSocket::EstablishConnection( const char * hostip, int port ) {

  int st = -1;
  if (!this->IPv6) {
    // set data for IPv4
    struct sockaddr_in sockAddress;
    // assign family and port for socket
    sockAddress.sin_family = AF_INET;
    sockAddress.sin_port = htons(port);
    
    // assign the given IP to socket 
    if (inet_pton(AF_INET, hostip, &sockAddress.sin_addr) <= 0) {
      throw std::runtime_error( "Invalid IPv4 address");
    }
    // attempt to connect with built socket
    st = connect(this->idSocket, (struct sockaddr*) &sockAddress,
    sizeof(sockAddress));

  }
  if ( -1 == st ) {
     // throw std::runtime_error( "VSocket::EstablishConnection");
  }
  return st;

}


/**
 * EstablishConnection method
 *   use "connect" Unix system call
 *
 * @param      char * host: host address**
 * @param      char * service: process address, example "http"
  *
**/
int VSocket::EstablishConnection( const char *host, const char *service ) {
  int st = -1;
  // different way for IPv6
  struct addrinfo hints,*results,*res;
  memset(&hints, 0, sizeof(hints));
  
  // assign info to hints
  hints.ai_family = PF_INET6;
  if (this->type == 's' || this->type == 'd') {
    hints.ai_socktype = (this->type == 's') ? SOCK_STREAM : SOCK_DGRAM;
  }
  // attempt to get address from the dns notation and service
  // httpServer adjusted in port 8080 by default, and localhost given by ::1
  int status = getaddrinfo(host, service, &hints, &results); 

  if (status != 0) {
    throw std::runtime_error( "VSocket::EstablishConnection -- getaddrinfo()" );
  }
  // try to connect with the found results   
  for (res = results; res != nullptr; res = res->ai_next) {    
    st = connect(this->idSocket, res->ai_addr, res->ai_addrlen);
    if (st != -1) break;
  }
  if (st == -1) {
    throw std::runtime_error( "VSocket::EstablishConnection -- connect()");
  }
  freeaddrinfo(results);
  return st;

}

/**
 * Bind method
 *    use "bind" Unix system call (man 3 bind) (server mode)
 *
 * @param      int port: bind a unamed socket to a port defined in sockaddr structure
 *
 *  Links the calling process to a service at port
 *
**/
int VSocket::Bind( int port, const char * address ) {
  this->port = port;
  int st = -1;
  // set the respective struct for ipv6 (or ipv4)
  if (this->IPv6) { 
    struct sockaddr_in6 serverAddr6;
    serverAddr6.sin6_family = PF_INET6;
    serverAddr6.sin6_port = htons(port);
    serverAddr6.sin6_addr = in6addr_any;
    // note: memset was removed because it crashed the binding function

    st = bind(this->idSocket, (struct sockaddr*) &serverAddr6, 
      sizeof(serverAddr6));
  } else {
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, address, &serverAddr.sin_addr);
    memset(serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero));
    
    st = bind(this->idSocket, (struct sockaddr*) &serverAddr, 
      sizeof(serverAddr));
  }

  if (st == -1) {
    // for utility, if it fails attach the detail of the reason
    std::cerr << "Bind failed: " << strerror(errno) << std::endl;
    throw std::runtime_error( "VSocket::Bind" );
  }
  
  return st;
}

/**
  * MarkPassive method
  *    use "listen" Unix system call (man listen) (server mode)
  *
  * @param int backlog: defines the maximum length to which the
  * queue of pending connections for this socket may grow
  *
  *  Establish socket queue length
  *
 **/
int VSocket::MarkPassive( int backlog ) {
  int st = -1;
  st = listen(this->idSocket, backlog);
  if (st == -1) {
    std::cerr << "Listen failed: " << strerror(errno) << std::endl;
    throw std::runtime_error( "VSocket::MarkPassive" );
  }
   return st;
}

/**
  * WaitForConnection method
  *    use "accept" Unix system call (man 3 accept) (server mode)
  *
  *
  *  Waits for a peer connections, return a sockfd of the connecting peer
  *
 **/
int VSocket::WaitForConnection( struct sockaddr_in* addr ) {
  int st = -1;
  //struct sockaddr_in addr;
  socklen_t addrLen = sizeof(addr);  

  st = accept(this->idSocket, (struct sockaddr*) addr, &addrLen); 
  if (st == -1) {
    std::cerr << "Acept failed: " << strerror(errno) << std::endl;
    throw std::runtime_error( "VSocket::WaitForConnection" );
  }

  return st;
}

/**
  * Shutdown method
  *    use "shutdown" Unix system call (man 3 shutdown) (server mode)
  *
  *
  *  cause all or part of a full-duplex connection on the socket associated with the file descriptor socket to be shut down
  *
 **/
int VSocket::Shutdown( int mode ) {
   int st = -1;
   st = shutdown(this->idSocket, mode);
   if (st == -1) {
    throw std::runtime_error( "VSocket::Shutdown" );
   }
   return st;

}

/**
 *  sendTo method
 *
 *  @param	const void * buffer: data to send
 *  @param	size_t size data size to send
 *  @param	void * addr address to send data
 *
 *  Send data to another network point (addr) without connection (Datagram)
 *
**/
size_t VSocket::sendTo( const void * buffer, size_t size, void * addr ) {
  int st = -1;
  // As for C++, it's better to use reinterpret-casting
  
  if (this->IPv6) { // for IPv6, use the respective structs for type
    const struct sockaddr_in6* addr_in = reinterpret_cast<const
    struct sockaddr_in6*>(addr);
  
  st = sendto(this->idSocket, buffer, size, 0, reinterpret_cast<const 
    struct sockaddr*>(addr_in), sizeof(*addr_in));

  } else { // for ipv4
     const struct sockaddr_in* addr_in = reinterpret_cast<const 
    struct sockaddr_in*>(addr);
  
    // Now it is reinterpretated again to the expected type
    st = sendto(this->idSocket, buffer, size, 0,
      reinterpret_cast<const struct sockaddr*>(addr_in), sizeof(*addr_in));
  }
  
  if (st == -1) {
    std::cerr << "Sendto failed: " << strerror(errno) << std::endl;
    throw std::runtime_error( "VSocket::sendTo" );
  }
  return st; 
}


/**
 *  recvFrom method
 *
 *  @param	const void * buffer: data to send
 *  @param	size_t size data size to send
 *  @param	void * addr address to receive from data
 *
 *  @return	size_t bytes received
 *
 *  Receive data from another network point (addr) without connection 
 *    (Datagram)
 *
**/
size_t VSocket::recvFrom( void * buffer, size_t size, void * addr ) {
  int st = -1;
  // set a variable of the expected type
  socklen_t len = sizeof(( struct sockaddr*)addr);
  st = recvfrom(this->idSocket, buffer, size, 0, ( struct sockaddr*)addr,
    &len);
  if (st == -1) {
    throw std::runtime_error( "VSocket::recvFrom" );
  }
  return st;

}