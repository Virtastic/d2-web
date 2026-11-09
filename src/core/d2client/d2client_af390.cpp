// 1.13c D2Client+0xAF390: packet handler read off the disassembly (ECX = unit, EDX = packet; request {w6, w8, b10, 2, 0, 4, b11}).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_pkt_env.h"
namespace {
template <class T> T rd(const uint8_t* p, uint32_t o) { T v; std::memcpy(&v, p + o, sizeof v); return v; }
}
extern "C" {
// 1.13c D2Client+0xAF390
EMSCRIPTEN_KEEPALIVE void d2c_af390(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p) {
    const uint32_t l[7] = { rd<uint16_t>(p, 6), rd<uint16_t>(p, 8), rd<uint8_t>(p, 0xa), 2, 0, 4, rd<uint8_t>(p, 0xb) };
    e->f2A750(rd<uint8_t>(p, 5), unit, l);
}
}
