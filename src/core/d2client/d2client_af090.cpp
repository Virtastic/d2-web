// 1.13c D2Client+0xAF090: packet handler read off the disassembly (ECX = packet; unit lookup, then 0xc9030 with five words).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_pkt_env.h"
namespace {
template <class T> T rd(const uint8_t* p, uint32_t o) { T v; std::memcpy(&v, p + o, sizeof v); return v; }
}
extern "C" {
// 1.13c D2Client+0xAF090
EMSCRIPTEN_KEEPALIVE void d2c_af090(const D2cPktEnv* e, const uint8_t* p) {
    const uint32_t t = rd<uint8_t>(p, 1);
    if (!e->lookup(rd<uint32_t>(p, 2), e->unitTable + (uintptr_t)t * 0x80 * sizeof(uintptr_t), t)) return;
    const uint32_t w = rd<uint16_t>(p, 0xb), c = rd<uint16_t>(p, 0xd);
    if (!w || !c) return;
    e->f79030(rd<uint16_t>(p, 6), rd<uint8_t>(p, 0xa), w, c, rd<uint16_t>(p, 0xf));
}
}
