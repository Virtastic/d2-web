// 1.13c D2Client+0xAF2B0: packet handler read off the disassembly (ECX = unit, EDX = packet). Null unit or packet
// returns. Builds the 7-dword request {word +5, word +7, byte +9, 2, 0, 4, 0}, places the unit through 0x2A1B0
// (x = word +5, y = word +7), bumps unit stat 0x148 by one (#10973 read, #10887 write, layer 0) and hands the
// request to 0x2A750 with kind 7.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_pkt_env.h"
namespace {
template <class T> T rd(const uint8_t* p, uint32_t o) { T v; std::memcpy(&v, p + o, sizeof v); return v; }
}
extern "C" {
// 1.13c D2Client+0xAF2B0
EMSCRIPTEN_KEEPALIVE void d2c_af2b0(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p) {
    if (!unit || !p) return;
    const uint32_t l[7] = { rd<uint16_t>(p, 5), rd<uint16_t>(p, 7), rd<uint8_t>(p, 9), 2, 0, 4, 0 };
    e->f2A1B0(rd<uint16_t>(p, 5), unit, rd<uint16_t>(p, 7), 0, 0, 0);
    e->setStat(unit, 0x148, e->getStat(unit, 0x148, 0) + 1, 0);
    e->f2A750(7, unit, l);
}
}
