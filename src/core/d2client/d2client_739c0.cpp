// 1.13c D2Client+0x739C0 -- skill to missile spawn request via the skill-desc row, then flags the unit.
// ECX = unit (EBX), EDX = skill index.
#include <emscripten.h>
#include "d2c_73930_env.h"
using namespace d2c73;

extern "C" EMSCRIPTEN_KEEPALIVE void d2client_739c0(const D2c73Env* e, uint32_t unit, int32_t skill) {
    const uint32_t rec = skillRec(e, skill);
    if (!rec) return;
    const int32_t di = rd16s(e, rec + 0x38);
    if (di < 0 || di >= (int32_t)rd32(e, e->tab + 0xba0)) return;
    const uint32_t desc = (uint32_t)di * 0x23cu + rd32(e, e->tab + 0xb98);
    if (!desc) return;
    const int32_t mis = rd16s(e, desc + 0xea);
    if (mis < 0 || mis >= (int32_t)rd32(e, e->tab + 0xb6c)) return;
    uint32_t b[23] = {};
    b[1] = unit; b[2] = unit; b[4] = (uint32_t)mis; b[11] = (uint32_t)di; b[12] = 1;
    C(e, 0xb30f0, 0, ptr(b));
    e->blk(e->ctx, 0xb30f0, b, 23);
    if (unit) {
        wr32(e, unit + 0xc8, rd32(e, unit + 0xc8) | 0x40000u);
        wr32(e, unit + 0xc4, rd32(e, unit + 0xc4) | 0x20u);
    }
}
