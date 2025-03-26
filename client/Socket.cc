 #include <sys/socket.h>         // sockaddr_in
 #include <arpa/inet.h>          // ntohs
 #include <unistd.h>		// write, read
 #include <cstring>
 #include <stdexcept>
 #include <stdio.h>		// printf
 
 #include "Socket.h"		// Derived class
 
/**
* @brief Constructor of the class Socket
* @param t type of the socket
* @param IPv6 boolean value to indicate if the socket is IPv6
*/
 Socket::Socket( char t, bool IPv6 ){
  this->BuildSocket( t, IPv6 );      // Call base class constructor
 }
 
/**
 * @brief Destructor of the class Socket
 */
 Socket::~Socket() {
  close(this->idSocket);
 }
 
/**
* @brief Method to make a connection
* @param hostip host ip
* @param port port
*/
int Socket::MakeConnection( const char * hostip, int port ) {
 
  int connectionResult = this->EstablishConnection( hostip, port );
    
  return connectionResult;
 }
 
/**
* @brief Method to make a connection
* @param host host name
* @param service service name
*/
 int Socket::MakeConnection( const char *host, const char *service ) {
  
  int  connectionResult = this->EstablishConnection( host, service );
  
  return connectionResult;
 }
 
/**
* @brief Method to read from the socket
* @param buffer buffer
* @param size size of the buffer
*/
 size_t Socket::Read( void * buffer, size_t size ) {
  
  return read(this->idSocket, buffer, size);
 }
 
/**
  * @brief Method to write to the socket
  * @param buffer buffer 
  * @param size size of the buffer
  */
 size_t Socket::Write( const void * buffer, size_t size ) {

  return write(this->idSocket, buffer, size);
 }
 
/**
  * @brief Method to write to the socket
  * @param text text
  */
 size_t Socket::Write( const char * text ) {

  size_t size = strlen(text); // almacena la longitud del texto
  void* buffer = (void*) text; // castea text en void*

  return this->Write(buffer, size);
 }
 