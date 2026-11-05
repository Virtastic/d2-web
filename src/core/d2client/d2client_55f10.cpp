// 1.13c D2Client+0x55F10 -- item-group summon dispatch (EAX = unit, EBX = owner; stack p1, p2; ret 8; void). Only when
// 0xC19C (unit) is 5: k = 0xC808 (unit) indexes the sgptDataTables item-type table (+0xC18, count +0xC1C, stride 0x1B8; out of
// range or null: nothing). For the six body slots T = {0xA5..0xAA} (.rdata 0xD2E50): it = 0xC1C0 (owner, T[i]); the first whose
// 0xC4B4 (it, 0x47, 0) equals the row's s16 +0x2C gets 0x55B20 (ECX = 0; owner, p1, p2, 0, T[i], 0, 1) and ends the walk.
#include "cl_env.h"
namespace { const uint32_t kSlot[6] = { 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa }; }
extern "C" void d2c_55f10(uint32_t unit, uint32_t owner, uint32_t p1, uint32_t p2, const cl::Env& e) {
    if (e.H(0xc19c, unit) != 5) return;
    int32_t k = (int32_t)e.H(0xc808, unit);
    if (k < 0 || k >= (int32_t)e.R(e.dt + 0xc1c)) return;
    uint32_t row = e.R(e.dt + 0xc18) + (uint32_t)k * 0x1b8;
    if (!row) return;
    for (int i = 0; i < 6; i++) {
        uint32_t it = e.H(0xc1c0, owner, kSlot[i]);
        if (!it || e.R16s(row + 0x2c) != (int32_t)e.H(0xc4b4, it, 0x47, 0)) continue;
        const uint32_t w[7] = { owner, p1, p2, 0, kSlot[i], 0, 1 };
        e.Xs(0, 1);
        e.H(0x55b20, w[0], w[1], w[2], w[3], w[4], 0, 0);
        if (e.blk) e.blk(e.ctx, 0x55b20, (uint32_t)(uintptr_t)w, 7);
        return;
    }
}
