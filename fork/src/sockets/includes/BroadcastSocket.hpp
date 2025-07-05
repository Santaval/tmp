/**
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2025-i
 *  Grupos: 1 y 3
 *
 ****** BroadcastSocket class interface
 *
 * (Fedora version)
**/

#ifndef BroadcastSocket_hpp
#define BroadcastSocket_hpp

#include "Socket.hpp"

class BroadcastSocket : public Socket {
public:
    BroadcastSocket(bool ipv6 = false);
    ~BroadcastSocket() override;

    int EnableBroadcast();
    size_t Broadcast(const void* buffer, size_t size, int port, const char* broadcast_ip = "255.255.255.255");
};

#endif
