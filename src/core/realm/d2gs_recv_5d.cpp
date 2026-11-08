#include "d2gs_party.h"

#include <emscripten.h>

// Client 0x5d, 7 bytes: [0x5d][command][on/off][target unit id u32].  1.13c's handler answers 3 for
// a wrong size, 2 for a command above 10 and otherwise returns the party command's result (3 when
// the command was refused).  The issuer is the connected client.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_recv_5d_from(uint32_t issuerId, const uint8_t* packet, int size) {
    if (size != 7) return 3;
    if (packet[1] > 10) return 2;
    const uint32_t target = (uint32_t)packet[3] | ((uint32_t)packet[4] << 8) | ((uint32_t)packet[5] << 16) | ((uint32_t)packet[6] << 24);
    const int issuer = d2gs_party::find(issuerId);
    if (issuer < 0) return 0;
    return d2gs_party::command(issuer, packet[1], target, packet[2]);
}

extern "C" EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_recv_5d(const uint8_t* packet, int size) {
    return d2_realm_d2gs_recv_5d_from(d2gs_party::g.self, packet, size);
}
