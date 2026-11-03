// 1.13c D2Client+0x73930 -- skill to missile spawn request, no prep.
// ECX = unit, EDX = skill index. Builds the 23-dword request block and hands it to 0xB30F0 (ECX = block).
#include <emscripten.h>
#include "d2c_73930_env.h"
using namespace d2c73;

extern "C" EMSCRIPTEN_KEEPALIVE void d2client_73930(const D2c73Env* e, uint32_t unit, int32_t skill) {
    const uint32_t rec = skillRec(e, skill);
    if (!rec) return;
    const int32_t mis = rd16s(e, rec + 0x3a);
    if (mis < 0 || mis >= (int32_t)rd32(e, e->tab + 0xb6c)) return;
    const int32_t sd = rd16s(e, rec + 0x38);
    uint32_t b[23] = {};
    b[1] = unit; b[2] = unit; b[4] = (uint32_t)mis; b[11] = sd < 0 ? 0 : (uint32_t)sd; b[12] = 1;
    C(e, 0xb30f0, 0, ptr(b));
    e->blk(e->ctx, 0xb30f0, b, 23);
}
