// 1.13c D2Client+0xAF160: packet handler read off the disassembly (ECX = unit, EDX = packet; 7-dword request, kind 0x15).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_pkt_env.h"
namespace {
template <class T> T rd(const uint8_t* p, uint32_t o) { T v; std::memcpy(&v, p + o, sizeof v); return v; }
}
extern "C" {
// 1.13c D2Client+0xAF160
EMSCRIPTEN_KEEPALIVE void d2c_af160(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p) {
    if (!unit) return;
    const uint32_t l[7] = { rd<uint32_t>(p, 6), 0xFFFFFFFFu, rd<uint16_t>(p, 0xb), rd<uint16_t>(p, 0xd), rd<uint8_t>(p, 0xa), 0, 0 };
    e->f2A750(0x15, unit, l);
}
}
