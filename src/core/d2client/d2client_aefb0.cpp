// 1.13c D2Client+0xAEFB0: packet handler (ECX = packet). Read off the disassembly: both callees take EAX = the first unit and the same six
// stack words; the data-table record count / array sit at +0xBA0 / +0xB98 of *sgptDataTables, the record stride is 0x23C, the test byte is rec+4.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_aefb0.h"
namespace {
template <class T> T rd(const uint8_t* p, uint32_t o) { T v; std::memcpy(&v, p + o, sizeof v); return v; }
}
extern "C" {
// 1.13c D2Client+0xAEFB0
EMSCRIPTEN_KEEPALIVE void d2c_aefb0(const D2cAEFB0Env* e, const uint8_t* p) {
    const uint32_t t1 = rd<uint8_t>(p, 6);
    const uintptr_t u1 = e->lookup(rd<uint32_t>(p, 7), e->unitTable + (uintptr_t)t1 * 0x80 * sizeof(uintptr_t), t1);
    if (!u1) return;
    const uint32_t t2 = rd<uint8_t>(p, 0xb);
    const uintptr_t u2 = e->lookup(rd<uint32_t>(p, 0xc), e->unitTable + (uintptr_t)t2 * 0x80 * sizeof(uintptr_t), t2);
    const int32_t i = rd<int16_t>(p, 2);
    const int32_t w4 = rd<int16_t>(p, 4);
    const uint32_t d10 = rd<uint32_t>(p, 0x10), d14 = rd<uint32_t>(p, 0x14);
    if (i < 0) return;
    const uint8_t* tab = (const uint8_t*)*e->sgptDataTables;
    if (i >= rd<int32_t>(tab, 0xba0)) return;
    const uint32_t rec = (uint32_t)i * 0x23c + rd<uint32_t>(tab, 0xb98);
    if (!rec) return;
    const uint8_t b = *(const uint8_t*)(uintptr_t)(rec + 4);
    const uint32_t b1 = rd<uint8_t>(p, 1);
    if (e->gdwBitMasks[8] & b) e->f52950(u1, u2, i, w4, d10, d14, b1);
    else e->f51B10(u1, u2, i, w4, d10, d14, b1);
}
}
