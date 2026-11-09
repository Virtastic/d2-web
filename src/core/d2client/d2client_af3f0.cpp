// 1.13c D2Client+0xAF3F0: packet handler read off the disassembly (ECX = unit, EDX = packet; 0x2A1B0 (EAX = w6, ESI = unit) then 0x2A750 with a 7-dword request).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_pkt_env.h"
namespace {
template <class T> T rd(const uint8_t* p, uint32_t o) { T v; std::memcpy(&v, p + o, sizeof v); return v; }
}
extern "C" {
// 1.13c D2Client+0xAF3F0
EMSCRIPTEN_KEEPALIVE void d2c_af3f0(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p) {
    const uint32_t l[7] = { rd<uint8_t>(p, 0xa), rd<uint32_t>(p, 0xb), rd<uint8_t>(p, 0xf), rd<uint8_t>(p, 0x11), (uint32_t)(int32_t)rd<int16_t>(p, 0x12), rd<uint8_t>(p, 0x14), rd<uint8_t>(p, 0x10) };
    e->f2A1B0(rd<uint16_t>(p, 6), unit, rd<uint16_t>(p, 8), 0, 0, 0);
    e->f2A750(rd<uint8_t>(p, 5), unit, l);
}
}
