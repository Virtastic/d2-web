// 1.13c D2Client+0xAF1D0: packet handler read off the disassembly (ECX = unit, EDX = packet; 7-dword request, kind 0x16).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_pkt_env.h"
namespace {
template <class T> T rd(const uint8_t* p, uint32_t o) { T v; std::memcpy(&v, p + o, sizeof v); return v; }
}
extern "C" {
// 1.13c D2Client+0xAF1D0
EMSCRIPTEN_KEEPALIVE void d2c_af1d0(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p) {
    if (!unit) return;
    const uint32_t l[7] = { rd<uint16_t>(p, 6), 0xFFFFFFFFu, rd<uint8_t>(p, 9), rd<uint32_t>(p, 0xa), rd<uint8_t>(p, 8), 0, 0 };
    e->f2A750(0x16, unit, l);
}
}
