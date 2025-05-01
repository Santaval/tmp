#include <openssl/ssl.h>
#include <openssl/err.h>

#include <stdexcept>

#include "SSLSocket.hpp"
#include "Socket.hpp"

/**  
 * @brief Constructor of the class SSLSocket
 * @param IPv6 boolean value to indicate if the socket is IPv6
 */
SSLSocket::SSLSocket( bool IPv6 ) {

   this->BuildSocket( 's', IPv6 );

   this->SSLContext = nullptr;
   this->SSLStruct = nullptr;

   this->Init();					// Initializes to client context
}

/**  
 * @brief Constructor of the class SSLSocket
 * @param certFileName certificate file name
 * @param keyFileName key file name
 * @param IPv6 boolean value to indicate if the socket is IPv6
 */
SSLSocket::SSLSocket( char * certFileName, char * keyFileName, bool IPv6 ) {
}


/**  
 * @brief Constructor of the class SSLSocket
 * @param id socket id
 */
SSLSocket::SSLSocket( int id ) {
   this->BuildSocket( id );
}

/**  
 * @brief Destructor of the class SSLSocket
 */
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
 * @brief Method to initialize the SSL socket
 */
void SSLSocket::Init( bool serverContext ) { // Create a SSL socket, a new context must be created before
   SSL * ssl = nullptr;

   this->InitContext( serverContext );
   
   ssl = SSL_new((SSL_CTX *) this->SSLContext);
   
   this->SSLStruct = (void *) ssl;

}

/**  
 * @brief Method to initialize the SSL context
 */
void SSLSocket::InitContext( bool serverContext ) {
   const SSL_METHOD * method; // almacena el tipo de protocolo que sera utilizado
   SSL_CTX * context; // contexto ssl, permite establecer una conexion segura

   method = TLS_client_method();
   context = SSL_CTX_new(method);
   this->SSLContext = (void *) context;
}

/**
 * @brief Method to load the certificates
 * @param certFileName certificate file name
 * @param keyFileName key file name
 */
void SSLSocket::LoadCertificates( const char * certFileName, const char * keyFileName ) {
}

/**  
 * @brief Method to establish a connection
 * @param hostName host name
 * @param port port number
 */
int SSLSocket::MakeConnection( const char * hostName, int port ) {
   int st = -1;
   
   st = this->EstablishConnection(hostName, port);
   SSL_set_fd((SSL *)this->SSLStruct, this->idSocket);
   SSL_connect((SSL *)this->SSLStruct);

   if ( -1 == st ) {
      throw std::runtime_error( "SSLSocket::MakeConnection( const char *, int )" );
   }

   return st;
}

/**  
 * @brief Method to establish a connection
 * @param host host name
 * @param service service name
 */
int SSLSocket::MakeConnection( const char * host, const char * service ) {
   int st = -1;
   st = this->EstablishConnection( host, service );

   SSL_set_fd((SSL *)this->SSLStruct, this->idSocket);
   SSL_connect((SSL *)this->SSLStruct);

   if ( -1 == st ) {
      throw std::runtime_error( "SSLSocket::MakeConnection( const char *, const char * )" );
   }

   return st;  
}

/**  
 * @brief Method to read from the socket
 * @param buffer buffer to store the data
 * @param size size of the buffer
 */
size_t SSLSocket::Read( void * buffer, size_t size ) {
   int st = -1;
   st = SSL_read((SSL *) this->SSLStruct, buffer, size);
   if ( -1 == st ) {
      throw std::runtime_error( "SSLSocket::Read( void *, size_t )" );
   }

   return st;
}

/**  
 * @brief Method to read from the socket
 */
size_t SSLSocket::Write( const char * string ) {
   int st = -1;  
   size_t size = strlen(string);

   st = SSL_write((SSL *) this->SSLStruct, string, size);

   if ( -1 == st ) {
      throw std::runtime_error( "SSLSocket::Write( const char * )" );
   }
   return st;  
}

/**  
 * @brief Method to write to the socket
 * @param buffer buffer
 * @param size size of the buffer
 */
size_t SSLSocket::Write( const void * buffer, size_t size ) {
   int st = -1;
   st = SSL_write((SSL *) this->SSLStruct, buffer, size);
   if ( -1 == st ) {
      throw std::runtime_error( "SSLSocket::Write( const void *, size_t )" );
   }
   return st;
}

/**  
 * @brief Method
 */
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
 * @brief Method
 */
const char * SSLSocket::GetCipher() {
   return SSL_get_cipher( reinterpret_cast<SSL *>( this->SSLStruct ) );
}