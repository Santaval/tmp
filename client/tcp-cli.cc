#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "VSocket.h"
#include "Socket.h"
#include "SSLSocket.h"

// This is a client that connects to a server and sends a request
// The server is specified by the IP address and port number
// The request is a HTTP GET request
// The client reads the response and prints it to the screen
int main( int argc, char * argv[] ) {
   const char * whalev6 = "0:0:0:0:0:ffff:8efa:504e%enp6s0";
   const char * whalev4 = "163.178.104.62";
   const char * request = "GET /aArt/index.php?disk=Disk-01&fig=whale-1.txt\r\nHTTP/v1.1\r\nhost: redes.ecci\r\n\r\n";
   VSocket * client;
   char a[512];
   int ipVer = 4;  // 4 = IPv4, 6 = IPv6
   int ssl = 0;  // 0 = non-SSL, 1 = SSL

   memset( a, 0, 512 );	// Clear the buffer

   if ( argc > 2 ) {
      ipVer = atoi( argv[ 1 ] );
      ssl = atoi( argv[ 2 ] );
   } else {
      if ( argc > 1 ) {
         ipVer = atoi( argv[ 1 ] );
      }
   }

   if ( ipVer != 6 ) {
      ipVer = 4;
   }
   if ( ssl != 1 ) {
      ssl = 0;
   }

   if ( ssl ) {
      printf( "Connecting SSL" );
      if ( 4 == ipVer ) {
         client = new SSLSocket();			// Create an IPv4 TCP SSL socket
         client->MakeConnection( whalev4, 443);
         printf( " IPv4\n" );
      } else {
         client = new SSLSocket( true );	// Create an IPv6 TCP SSL socket
         client->MakeConnection( whalev6, 443 );
         printf( " IPv6\n" );
      }
   } else {							// Non SSL socket
      printf( "Connecting non-SSL" );
      if ( 4 == ipVer ) {
         client = new Socket( 's' );		// Create an IPv4 TCP socket
         client->MakeConnection( whalev4, 80 );
         printf( " IPv4\n" );
      } else {
         client = new Socket( 's', true );	// Create an IPv6 TCP socket
         client->MakeConnection( whalev6, 80 );
         printf( " IPv6\n" );
      }
   }

   client->Write( (char *) request, strlen(request) );
   client->Read( a, 511 );
   printf( "%s\n", a);

}