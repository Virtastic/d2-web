// 1.13c D2Client+0xAEDF0: server stat packet 0x19..0x1F applied to the hero.
// The server's stat packets 0x19..0x1F written onto the hero: ECX = packet, byte 0 is the opcode. Any other opcode
// falls through the switch and writes stat -1 with value 0 (retail does the same; the 0x20 form is +0xAED80).
#include <cstdint>
#include <emscripten.h>
#include "d2client_aedf0.h"

namespace {
uint32_t rd16(const uint8_t* p) { return uint32_t(p[0]) | uint32_t(p[1]) << 8; }
uint32_t rd32(const uint8_t* p) { return rd16(p) | rd16(p + 2) << 16; }
}

extern "C" {
// 1.13c D2Client+0xAEDF0
EMSCRIPTEN_KEEPALIVE void d2c_aedf0(const D2cAEDF0Env* e, const uint8_t* packet) {
    void* hero = e->hero;
    if (!hero) {                       // retail: Fog 10265 / 10024 then _exit(-1); the client does not continue
        e->fatal();
        return;
    }
    int32_t stat = -1;
    uint32_t value = 0;
    switch (packet[0]) {
    case 0x19: stat = 14; value = uint32_t(e->getStat(hero, 14, 0)) + packet[1]; break;
    case 0x1a: stat = 13; value = uint32_t(e->getStat(hero, 13, 0)) + packet[1]; break;
    case 0x1b: stat = 13; value = uint32_t(e->getStat(hero, 13, 0)) + rd16(packet + 1); break;
    case 0x1c: stat = 13; value = rd32(packet + 1); break;
    case 0x1d: stat = packet[1]; value = packet[2]; break;
    case 0x1e: stat = packet[1]; value = rd16(packet + 2); break;
    case 0x1f: stat = packet[1]; value = rd32(packet + 2); break;
    default: break;
    }
    e->setStat(hero, stat, int32_t(value), 0);
    e->statEvent(stat, hero, int32_t(value));
}
}
