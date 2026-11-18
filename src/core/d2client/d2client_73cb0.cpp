// 1.13c D2Client+0x73CB0 -- cltdofunc 25 cast: owner missile from the skill-desc row, flag the unit, request 0xA1020.
// ECX = unit (EBP, may be 0), EDX = skill-desc index (EDI), one stack word (ret 4). Returns 1/0.
#include <emscripten.h>
#include "d2c_73930_env.h"
using namespace d2c73;

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2client_73cb0(const D2c73Env* e, uint32_t unit, int32_t skill, uint32_t arg) {
    const uint32_t row = descRec(e, skill);
    if (!row) return 0;
    const int32_t owner = (int32_t)C(e, 0x40550, (uint32_t)skill, 0, 0, 0, 0, 0, unit);
    if (owner < 0 || owner >= (int32_t)rd32(e, e->tab + 0xb6c)) return 0;
    if (unit) wr32(e, unit + 0xc4, rd32(e, unit + 0xc4) | 0x40u);
    const uint32_t r1 = C(e, 0xcb5c, 0, 0, 0, 0, 0, 0, (uint32_t)owner, arg);
    const uint32_t r2 = C(e, 0xc316, 0, 0, 0, 0, 0, 0, unit, rd32(e, row + 0x114), (uint32_t)skill, arg);
    C(e, 0xa1020, unit, 0, 0, r1 + r2, 0, 0, unit, (uint32_t)owner, 0xffffffffu, (uint32_t)skill, arg);
    return 1;
}
