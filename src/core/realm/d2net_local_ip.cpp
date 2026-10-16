// 1.13c D2Net+0x6B70 (ordinal 10030): local address of the server socket as a dotted quad.
// Retail: getsockname(sock) -> inet_ntoa(sin_addr) -> SStrCopy(dest, s, 0x7fffffff).
#include <cstdint>
#include <cstdio>

// sinAddr: sockaddr_in.sin_addr as read from the struct (network order bytes, first octet = lowest byte).
// Returns the length written, excluding the NUL.
int d2_realm_net_local_ip(char* dest, uint32_t sinAddr) {
    return snprintf(dest, 16, "%u.%u.%u.%u", sinAddr & 255, (sinAddr >> 8) & 255, (sinAddr >> 16) & 255, sinAddr >> 24);
}
