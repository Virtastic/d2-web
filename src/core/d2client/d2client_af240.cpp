// 1.13c D2Client+0xAF240: unit-queue handler (ECX = unit, EDX = packet); no-op for the local player, fatal for a null unit.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_unit_ports.h"
extern "C" {
uint16_t d2c_af240_pair[2];   // retail keeps the pair on its stack; 0x2A750 receives its address in l[1]
// 1.13c D2Client+0xAF240
// Returns retail's EAX: the packet address on the local-player path (EAX takes EDX at entry), else the callee's (void here, so 0).
EMSCRIPTEN_KEEPALIVE uint32_t d2c_af240(const D2cPlayerEnv* e, uintptr_t unit, const uint8_t* p) {
    if (unit == e->localPlayer) return (uint32_t)(uintptr_t)p;
    if (!unit) { e->fatal(); return 0; }
    uint16_t a, b, w8, wA;
    std::memcpy(&w8, p + 8, 2); std::memcpy(&wA, p + 10, 2);
    a = e->rd16(w8); b = e->rd16(wA);            // retail loads through the packet words as addresses
    d2c_af240_pair[0] = a; d2c_af240_pair[1] = b;
    uint32_t l[2] = { (uint32_t)(p[6] >> 1), (uint32_t)(uintptr_t)d2c_af240_pair };   // l[1] is the address of the pair
    e->pkt->f2A750(p[7], unit, l);
    return 0;
}
}
