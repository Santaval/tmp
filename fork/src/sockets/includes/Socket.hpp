/**
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2025-i
 *  Grupos: 1 y 3
 *
 ****** Socket class interface
*
* (Fedora version)
*
**/

#ifndef Socket_hpp
#define Socket_hpp

#include "VSocket.hpp"

class Socket : public VSocket {

	public:
		Socket( char, bool = false );
		Socket(int);
		~Socket() override;
		int MakeConnection( const char *, int ) override;
		int MakeConnection( const char *, const char * ) override;
		size_t Read( void *, size_t ) override;
		size_t Write( const void *, size_t ) override;
		size_t Write( const char * ) override;

		VSocket * AcceptConnection(struct sockaddr_in* addr) override;

	protected:

};

#endif

