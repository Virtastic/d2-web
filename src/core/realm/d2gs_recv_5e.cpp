#include "d2gs_party.h"

#include <emscripten.h>

// Client 0x5e, 6 bytes: [0x5e][command][target unit id u32] -- invite (6), cancel (7), accept (8),
// leave (9).  Wrong size answers 3, a command above 10 answers 2, anything else 0 (the party
// command's own refusal is not reported for this opcode).
extern "C" EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_recv_5e_from(uint32_t issuerId, const uint8_t* packet, int size) {
    if (size != 6) return 3;
    if (packet[1] > 10) return 2;
    const uint32_t target = (uint32_t)packet[2] | ((uint32_t)packet[3] << 8) | ((uint32_t)packet[4] << 16) | ((uint32_t)packet[5] << 24);
    const int issuer = d2gs_party::find(issuerId);
    if (issuer >= 0) d2gs_party::command(issuer, packet[1], target, 0);
    return 0;
}

extern "C" EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_recv_5e(const uint8_t* packet, int size) {
    return d2_realm_d2gs_recv_5e_from(d2gs_party::g.self, packet, size);
}
