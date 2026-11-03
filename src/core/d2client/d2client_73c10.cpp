// 1.13c D2Client+0x73C10 -- can the unit cast skill-desc row EDX on its current spot.
// ECX = unit (EBX), EDX = skill-desc index (ESI), one stack word (ret 4) unused. Returns 1/0.
#include <emscripten.h>
#include "d2c_73930_env.h"
using namespace d2c73;

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2client_73c10(const D2c73Env* e, uint32_t unit, int32_t skill) {
    const uint32_t cell = C(e, 0xc1f0, 0, 0, 0, 0, 0, 0, unit);
    if (!cell) return 0;
    uint32_t loc[2] = { 0, 0 };   // 0xa1340: x goes to the stack arg, y to [EDI]
    if (!C(e, 0xa1340, 0, 0, 0, unit, 0, ptr(&loc[1]), ptr(&loc[0]))) return 0;
    const uint32_t room = C(e, 0x1400, cell, 0, 0, loc[1], 0, loc[0]);
    if (!room) return 0;
    const uint32_t row = descRec(e, skill);
    if (!row) return 0;
    if (rd8(e, e->bm) & rd8(e, row + 5)) return 1;
    return C(e, 0xc208, 0, 0, 0, 0, 0, 0, room) ? 0 : 1;
}
