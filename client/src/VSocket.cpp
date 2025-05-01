 #include <sys/socket.h>
 #include <arpa/inet.h>		
 #include <stdexcept>             
 #include <cstring>		
 #include <netdb.h>			
 #include <unistd.h>
 #include <iostream>

 #include "VSocket.hpp"
 
 /**
  * @brief Builder of the class VSocket
  */
 void VSocket::BuildSocket(char t, bool IPv6) {
  this->domain = AF_INET; // IPv4 por defecto
  this->type = SOCK_STREAM; // TCP por defecto

  if (IPv6) {
      this->domain = AF_INET6; // Cambiar a IPv6
  }

  if (t == 'd') {
      this->type = SOCK_DGRAM; // Cambiar a UDP
  }

  this->idSocket = socket(this->domain, this->type, 0); // Crear el socket

  if (this->idSocket == -1) { // Validar errores
      throw std::runtime_error("VSocket::BuildSocket - Error al crear el socket");
  }

  std::cout << "Socket creado correctamente\n";
}
 
/**
* @brief Destructor of the class VSocket
*/
 VSocket::~VSocket() {
    this->Close();
 }
 
 
 /**
  * @brief Method to close the socket
  */    
 void VSocket::Close(){
 }
 
 /**
  * @brief Establish a connection with a server
  * @param hostip IP address of the server
  * @param port Port of the server
  * @return 0 if the connection was successful, -1 otherwise
  */
 int VSocket::EstablishConnection(const char *hostip, int port) {
  int st;

  if (this->domain == AF_INET6) {
      // IPv6
      struct sockaddr_in6 host6;
      memset(&host6, 0, sizeof(host6));

      host6.sin6_family = this->domain;
      host6.sin6_port = htons(port);

      st = inet_pton(this->domain, hostip, &host6.sin6_addr);
      if (st <= 0) {
          if (st == 0) {
              throw std::runtime_error("VSocket::EstablishConnection - Dirección IP inválida (IPv6)");
          } else {
              throw std::runtime_error("VSocket::EstablishConnection - Error en inet_pton (IPv6)");
          }
      }

      st = connect(idSocket, (sockaddr *)&host6, sizeof(host6));
      if (st == -1) {
          perror("VSocket::EstablishConnection - connect (IPv6)");
          throw std::runtime_error("VSocket::EstablishConnection - connect falló (IPv6)");
      }
  } else {
      // IPv4
      struct sockaddr_in host;
      memset(&host, 0, sizeof(host));

      host.sin_family = this->domain;
      host.sin_port = htons(port);

      st = inet_pton(this->domain, hostip, &host.sin_addr);
      if (st <= 0) {
          if (st == 0) {
              throw std::runtime_error("VSocket::EstablishConnection - Dirección IP inválida (IPv4)");
          } else {
              throw std::runtime_error("VSocket::EstablishConnection - Error en inet_pton (IPv4)");
          }
      }

      st = connect(idSocket, (sockaddr *)&host, sizeof(host));
      if (st == -1) {
          perror("VSocket::EstablishConnection - connect (IPv4)");
          throw std::runtime_error("VSocket::EstablishConnection - connect falló (IPv4)");
      }
  }

  return st;
}
 
/**
* @brief Establish a connection with a server
* @param host Hostname of the server
* @param service Service of the server
* @return 0 if the connection was successful, -1 otherwise
*/
 int VSocket::EstablishConnection(const char *host, const char *service) {
  int st;
  struct addrinfo hints, *result, *rp;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = this->domain;    // AF_INET o AF_INET6
  hints.ai_socktype = this->type;   // SOCK_STREAM o SOCK_DGRAM
  hints.ai_flags = 0;
  hints.ai_protocol = 0;           // Cualquier protocolo

  st = getaddrinfo(host, service, &hints, &result);
  if (st != 0) {
      throw std::runtime_error(std::string("VSocket::EstablishConnection - getaddrinfo: ") + gai_strerror(st));
  }

  for (rp = result; rp != nullptr; rp = rp->ai_next) {
      st = connect(this->idSocket, rp->ai_addr, rp->ai_addrlen);
      if (st == 0) {
          break; // Conexión exitosa
      }
      perror("VSocket::EstablishConnection - connect");
  }

  if (rp == nullptr) { // No se pudo conectar con ninguna dirección
      freeaddrinfo(result);
      throw std::runtime_error("VSocket::EstablishConnection - No se pudo conectar");
  }

  freeaddrinfo(result);
  return st;
}

 