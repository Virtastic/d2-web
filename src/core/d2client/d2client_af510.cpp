// 1.13c D2Client+0xAF510: packet handler read off the disassembly (ECX = unit, EDX = packet; 0x2A1B0 (EAX = w12, ESI = unit) then 0x2A750 with {w7, w9, b11}).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_pkt_env.h"
namespace {
template <class T> T rd(const uint8_t* p, uint32_t o) { T v; std::memcpy(&v, p + o, sizeof v); return v; }
}
extern "C" {
// 1.13c D2Client+0xAF510
EMSCRIPTEN_KEEPALIVE void d2c_af510(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p) {
    const uint32_t l[3] = { rd<uint16_t>(p, 7), rd<uint16_t>(p, 9), rd<uint8_t>(p, 0xb) };
    e->f2A1B0(rd<uint16_t>(p, 0xc), unit, rd<uint16_t>(p, 0xe), 0, 0, 0);
    e->f2A750(rd<uint8_t>(p, 6), unit, l);
}
}
