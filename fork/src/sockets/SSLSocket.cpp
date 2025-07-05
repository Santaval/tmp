/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  *  Socket class implementation
  *
  * (Fedora version)
  *
 **/
 


#include <stdexcept>

#include "includes/SSLSocket.hpp"

/**
  *  Class constructor
  *     use base class
  *
  *  @param     char t: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param     bool ipv6: if we need a IPv6 socket
  *
 **/
SSLSocket::SSLSocket(bool serverContext, bool IPv6 ) {

   this->BuildSocket( 's', IPv6 );

   this->SSLContext = nullptr;
   this->SSLStruct = nullptr;

   this->Init(serverContext);					// Initializes to client context

}


/**
  *  Class constructor
  *     use base class
  *
  *  @param     char t: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param     bool IPv6: if we need a IPv6 socket
  *
 **/
SSLSocket::SSLSocket( char * certFileName, char * keyFileName, bool IPv6 ) {
   this->IPv6 = IPv6;
   this->LoadCertificates(certFileName, keyFileName);
}


/**
  *  Class constructor
  *
  *  @param     int id: socket descriptor
  *
 **/
SSLSocket::SSLSocket( int id, SSL* ctx) {

   this->idSocket = id;
   this->SSLStruct = (void*) ctx;

}


/**
  * Class destructor
  *
 **/
SSLSocket::~SSLSocket() {

// SSL destroy
   if ( nullptr != this->SSLContext ) {
      SSL_CTX_free( reinterpret_cast<SSL_CTX *>( this->SSLContext ) );
   }
   if ( nullptr != this->SSLStruct ) {
      SSL_free( reinterpret_cast<SSL *>( this->SSLStruct ) );
   }

   this->Close();

}


/**
  *  SSLInit
  *     use SSL_new with a defined context
  *
  *  Create a SSL object
  *
 **/
void SSLSocket::Init( bool serverContext ) {
   SSL * ssl;

   this->InitContext( serverContext );
   ssl = SSL_new( (SSL_CTX *) this->SSLContext );
   this->SSLStruct = (void *) ssl;
}


/**
  *  InitContext
  *     use SSL_library_init, OpenSSL_add_all_algorithms, SSL_load_error_strings, TLS_server_method, SSL_CTX_new
  *
  *  Creates a new SSL server context to start encrypted comunications, this context is stored in class instance
  *
 **/
void SSLSocket::InitContext( bool serverContext ) {
   const SSL_METHOD * method;
   SSL_CTX * context;
   SSL_library_init();
   OpenSSL_add_all_algorithms();

   if ( serverContext ) {
      method = TLS_server_method();
   } else {
      method = TLS_client_method();
   }
   if ( nullptr == method ) {
      throw std::runtime_error( "SSLSocket::InitContext( bool )" );
   }
   context = SSL_CTX_new(method);
   this->SSLContext = (void *) context;
   if (!context) {
      throw std::runtime_error( "SSLSocket::InitContext( bool )" );
   }

   this->LoadCertificates("michael.cert", "michaelo.key");
   
}


/**
 *  Load certificates
 *    verify and load certificates
 *
 *  @param	const char * certFileName, file containing certificate
 *  @param	const char * keyFileName, file containing keys
 *
 **/
 void SSLSocket::LoadCertificates( const char * certFileName, const char * keyFileName ) {
   SSL_CTX* ctx = static_cast<SSL_CTX*>(this->SSLContext);

   if (ctx == nullptr) {
      printf("Could not load certificates: no valid SSL Context\n");
      return;
   }
   if (SSL_CTX_use_certificate_file(ctx, certFileName,
    SSL_FILETYPE_PEM) <= 0) {
      ERR_print_errors_fp(stderr);
      return;
    }

   if (SSL_CTX_use_PrivateKey_file(ctx, keyFileName,
    SSL_FILETYPE_PEM) <= 0) {
      ERR_print_errors_fp(stderr);
      return;
    }

    // Check if private key is valid
    if (!SSL_CTX_check_private_key(ctx)) {
      fprintf(stderr, "Private key does not match the public certificate\n");
      return;
    }
}

/**
  * AcceptiConnection method
  *    use base class to accept connections
  *
  *  @returns   a new class instance
  *
  *  Waits for a new connection to service (TCP mode: stream)
  *
 **/
VSocket * SSLSocket::AcceptConnection(struct sockaddr_in* addr){
   int id;
   VSocket * peer;
   
   id = this->WaitForConnection(addr);

   

    SSL* newSSL = SSL_new((SSL_CTX*) this->SSLContext); //
    if (!newSSL) {
        close(id);
        throw std::runtime_error("SSL_new failed");
    }

    SSL_set_fd(newSSL, id);

    int st = SSL_accept(newSSL);
    if (st <= 0) {
        SSL_free(newSSL);
        close(id);
        throw std::runtime_error("SSL_accept failed");
    }
    peer = new SSLSocket( id, newSSL);
    return peer;

}

/**
 *  Connect
 *     use SSL_connect to establish a secure conection
 *
 *  Create a SSL connection
 *
 *  @param	char * hostName, host name
 *  @param	int port, service number
 *
 **/
int SSLSocket::MakeConnection( const char * hostName, int port ) {
   int st;

    st = this->EstablishConnection( hostName, port );		// Establish a non ssl connection first
   if (st == -1) {
      throw std::runtime_error( "SSLSocket::MakeConnection( bool )" );
   }
   //  sets the file descriptor fd as the input/output facility for SSL
   st = SSL_set_fd((SSL*)this->SSLStruct, this->idSocket);
   if (st == 0) {
      printf("Failed setting fd\n");
      throw std::runtime_error( "SSLSocket::MakeConnection( bool )" );
   }
  
   st = SSL_connect((SSL*)this->SSLStruct); 
   // initiates the TLS/SSL handshake with a server
    
   if (st != 1) {
      SSL_get_error((SSL*)this->SSLStruct, st);
   }
   
   return st;

}


/**
 *  Connect
 *     use SSL_connect to establish a secure conection
 *
 *  Create a SSL connection
 *
 *  @param	char * hostName, host name
 *  @param	char * service, service name
 *
 **/
int SSLSocket::MakeConnection( const char * host, const char * service ) {
   int st;
   // Establish a non ssl connection first
   st = this->EstablishConnection( host, service );
    //  sets the file descriptor fd as the input/output facility for SSL
   if (st == -1) {
      throw std::runtime_error( "SSLSocket::MakeConnection( bool )" );
   }
   st = SSL_set_fd((SSL*)this->SSLStruct, this->idSocket);
   if (st == 0) {
      printf("Failed setting fd\n");
      throw std::runtime_error( "SSLSocket::MakeConnection( bool )" );
   }
   st = SSL_connect((SSL*)this->SSLStruct); 
   // initiates the TLS/SSL handshake with a server
   
   if (st != 1) {
      SSL_get_error((SSL*)this->SSLStruct, st);
   }
   
   return st;
   return st;

}


/**
  *  Read
  *     use SSL_read to read data from an encrypted channel
  *
  *  @param	void * buffer to store data read
  *  @param	size_t size, buffer's capacity
  *
  *  @return	size_t byte quantity read
  *
  *  Reads data from secure channel
  *
 **/
size_t SSLSocket::Read( void * buffer, size_t size ) {
   int st = -1;
   st = SSL_read((SSL*)this->SSLStruct, buffer, size);
   if ( st <= 0 ) {
      throw std::runtime_error( "SSLSocket::Read( void *, size_t )" );
   }

   return st;

}


/**
  *  Write
  *     use SSL_write to write data to an encrypted channel
  *
  *  @param	void * buffer to store data read
  *  @param	size_t size, buffer's capacity
  *
  *  @return	size_t byte quantity written
  *
  *  Writes data to a secure channel
  *
 **/
size_t SSLSocket::Write( const char * string ) {
   int st = -1;
   st = SSL_write((SSL *)this->SSLStruct, static_cast<const void*>(string), sizeof(string));
   if ( st <= 0 ) {
      throw std::runtime_error( "SSLSocket::Write( const char * )" );
   }

   return st;

}


/**
  *  Write
  *     use SSL_write to write data to an encrypted channel
  *
  *  @param	void * buffer to store data read
  *  @param	size_t size, buffer's capacity
  *
  *  @return	size_t byte quantity written
  *
  *  Reads data from secure channel
  *
 **/
size_t SSLSocket::Write( const void * buffer, size_t size ) {
   int st = -1;
   st = SSL_write((SSL *)this->SSLStruct, buffer, size);
   if ( st <= 0 ) {
      throw std::runtime_error( "SSLSocket::Write( void *, size_t )" );
   }

   return st;

}


/**
 *   Show SSL certificates
 *
 **/
void SSLSocket::ShowCerts() {
   X509 *cert;
   char *line;

   cert = SSL_get_peer_certificate( (SSL *) this->SSLStruct );		 // Get certificates (if available)
   if ( nullptr != cert ) {
      printf("Server certificates:\n");
      line = X509_NAME_oneline( X509_get_subject_name( cert ), 0, 0 );
      printf( "Subject: %s\n", line );
      free( line );
      line = X509_NAME_oneline( X509_get_issuer_name( cert ), 0, 0 );
      printf( "Issuer: %s\n", line );
      free( line );
      X509_free( cert );
   } else {
      printf( "No certificates.\n" );
   }

}


/**
 *   Return the name of the currently used cipher
 *
 **/
const char * SSLSocket::GetCipher() {

   return SSL_get_cipher( reinterpret_cast<SSL *>( this->SSLStruct ) );

}
