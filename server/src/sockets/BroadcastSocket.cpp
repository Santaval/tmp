/**
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2025-i
 *  Grupos: 1 y 3
 *
 ****** BroadcastSocket class implementation
 *
 * (Fedora version)
**/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <stdexcept>
#include "BroadcastSocket.hpp"

BroadcastSocket::BroadcastSocket(bool ipv6) : Socket('d', ipv6) {
    EnableBroadcast();
}

BroadcastSocket::~BroadcastSocket() {}

int BroadcastSocket::EnableBroadcast() {
    int broadcastEnable = 1;
    if (setsockopt(this->idSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        throw std::runtime_error("BroadcastSocket::EnableBroadcast failed");
    }
    return 0;
}

size_t BroadcastSocket::Broadcast(const void* buffer, size_t size, int port, const char* broadcast_ip) {
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(broadcast_ip);
    ssize_t sent = sendto(this->idSocket, buffer, size, 0, (struct sockaddr*)&addr, sizeof(addr));
    if (sent < 0) {
        throw std::runtime_error("BroadcastSocket::Broadcast failed");
    }
    return sent;
}
