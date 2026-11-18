// 1.13c D2Client+0xAF7C0: packet handler read off the disassembly (ECX = unit, EDX = packet). Places the unit
// through 0x2A1B0 (x = word +0xC, y = word +0xE) and tail-jumps into the 0xAF390 handler with the same registers.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_pkt_env.h"
namespace {
template <class T> T rd(const uint8_t* p, uint32_t o) { T v; std::memcpy(&v, p + o, sizeof v); return v; }
}
extern "C" {
// 1.13c D2Client+0xAF7C0
EMSCRIPTEN_KEEPALIVE void d2c_af7c0(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p) {
    e->f2A1B0(rd<uint16_t>(p, 0xc), unit, rd<uint16_t>(p, 0xe), 0, 0, 0);
    d2c_af390(e, unit, p);
}
}
