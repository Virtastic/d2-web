// 1.13c D2Client+0x69660: EDI = unit, EDX = index. The index must be in 0..count(table+0xBC0)
// with a row at table+0xBBC + index*0x84 (else fatal, line 0xCA9; table = **0x6FB7E474). Finds the first node of
// data(+0x54)+0x2C with +4 == index, or a type 4/9 node with +8 or +0xC == index, and unlinks it through 0x69180.
#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_69660(uint32_t unit, int32_t idx, uint32_t table, const Env* e) {
    uint32_t d = unit ? R(unit, 0x54) : 0;
    if (!d) return;
    if (idx < 0 || idx >= (int32_t)R(table, 0xBC0) || (uint32_t)(idx * 0x84) + R(table, 0xBBC) == 0) { e->fatal(0xCA9); return; }
    for (uint32_t n = R(d, 0x2C); n; n = R(n, 0xA4)) {
        uint32_t t = R(n, 0);
        if (R(n, 4) == (uint32_t)idx || ((t == 4 || t == 9) && (R(n, 8) == (uint32_t)idx || R(n, 0xC) == (uint32_t)idx))) {
            e->call(0x69180, 0, n, 0, 0, 0, unit, 0);
            return;
        }
    }
}
