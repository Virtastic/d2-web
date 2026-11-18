// 1.13c D2Client+0x73AA0 -- unit post-stat sync: gathers two unit-data words into a 7-dword block for 0x2A750 (ECX = 0x19, EDX = unit),
// then 0xC2B6(pData, 0). ECX = unit (ESI). Returns the 0xC2B6 result.
#include <emscripten.h>
#include "d2c_73930_env.h"
using namespace d2c73;

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2client_73aa0(const D2c73Env* e, uint32_t unit) {
    uint32_t L[7] = {};
    const int32_t type = (int32_t)rd32(e, unit);
    const uint32_t pd = rd32(e, unit + 0x2c);
    const bool direct = type == 2 || (type > 3 && type <= 5);
    L[0] = direct ? rd32(e, pd + 0xc) : (pd ? C(e, 0xc3be, 0, 0, 0, 0, 0, 0, pd) : 0);
    L[1] = direct ? rd32(e, pd + 0x10) : (pd ? C(e, 0xc2ec, 0, 0, 0, 0, 0, 0, pd) : 0);
    C(e, 0x2a750, 0, 0x19, unit, 0, 0, 0, ptr(L), 0);
    e->blk(e->ctx, 0x2a750, L, 7);
    return C(e, 0xc2b6, 0, 0, 0, 0, 0, 0, rd32(e, unit + 0x2c), 0);
}
