// 1.13c D2Client+0xAF340: packet handler read off the disassembly (ECX = unit, EDX = packet; request {b6, d7, b11, 2, 0, 4}).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_pkt_env.h"
namespace {
template <class T> T rd(const uint8_t* p, uint32_t o) { T v; std::memcpy(&v, p + o, sizeof v); return v; }
}
extern "C" {
// 1.13c D2Client+0xAF340
EMSCRIPTEN_KEEPALIVE void d2c_af340(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p) {
    const uint32_t l[6] = { rd<uint8_t>(p, 6), rd<uint32_t>(p, 7), rd<uint8_t>(p, 0xb), 2, 0, 4 };
    e->f2A750(rd<uint8_t>(p, 5), unit, l);
}
}
