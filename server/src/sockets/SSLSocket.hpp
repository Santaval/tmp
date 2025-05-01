#ifndef SSLSocket_h
#define SSLSocket_h

#include "VSocket.hpp"
#include <unistd.h>			// close
// SSL includes
#include <openssl/ssl.h>
#include <openssl/err.h>
/**
 * @brief Class to manage SSL connections
 */
class SSLSocket : public VSocket {

   public:
      SSLSocket(  bool serverContext, bool IPv6 = false); // Not possible to create with UDP, client constructor
      SSLSocket( char *, char *, bool = false );		// For server connections
      SSLSocket( int, SSL*);
      ~SSLSocket() override;
      int Connect( const char *, int );
      int Connect( const char *, const char * );
      size_t Write( const char * ) override;
      size_t Write( const void *, size_t ) override;
      size_t Read( void *, size_t ) override;
      void ShowCerts();
      const char * GetCipher();

      // added
      int MakeConnection( const char *, int ) override;
      int MakeConnection( const char *, const char * ) override;
      VSocket * AcceptConnection(struct sockaddr_in* addr) override;

   private:
      void Init( bool = false );		// Defaults to create a client context, true if server context needed
      void InitContext( bool );
      void LoadCertificates( const char *, const char * );

// Instance variables      
      void * SSLContext;				// SSL context
      void * SSLStruct;					// SSL BIO (Basic Input/Output)

};

#endif

