// 1.13c D2Client+0x69610: EDI = unit. Unlinks every node of data(+0x54)+0x2C through 0x69180
// (thiscall, next read first). No unit/data is fatal (line 0xCCF).
#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_69610(uint32_t unit, const Env* e) {
    uint32_t d = unit ? R(unit, 0x54) : 0;
    if (!d) { e->fatal(0xCCF); return; }
    for (uint32_t n = R(d, 0x2C); n;) { uint32_t next = R(n, 0xA4); e->call(0x69180, 0, n, 0, 0, 0, unit, 0); n = next; }
}
