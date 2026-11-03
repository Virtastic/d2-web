// 1.13c D2Client+0x73DF0 -- skill-cast marker: owner missile from the skill-desc row, flag the unit, then 0xa1410 / 0xa1540.
// ECX = unit (EBX, may be 0), EDX = skill-desc index (ESI), one stack word (ret 4). Returns 1/0.
#include <emscripten.h>
#include "d2c_73930_env.h"
using namespace d2c73;

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2client_73df0(const D2c73Env* e, uint32_t unit, int32_t skill, uint32_t arg) {
    const uint32_t row = descRec(e, skill);
    if (!row) return 0;
    const int32_t owner = (int32_t)C(e, 0x40550, (uint32_t)skill, 0, 0, 0, 0, 0, unit);
    if (owner < 0 || owner >= (int32_t)rd32(e, e->tab + 0xb6c)) return 0;
    if (unit) wr32(e, unit + 0xc4, rd32(e, unit + 0xc4) | 0x40u);
    if (!C(e, 0xa1410, (uint32_t)owner, unit)) return 0;
    return C(e, 0xa1540, unit, 0, 0, 0, 0, 0, (uint32_t)owner, (uint32_t)skill, arg) != 0;
}
