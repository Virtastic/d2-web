// 1.13c D2Game+0x95FE0: ecx = c, edx = unit, stack idx, a1 (ret 8). idx picks a skills row (dt+0xB98, stride 0x23C, count dt+0xBA0); 0xE07A0(eax = unit, ecx = c); target = 0xA598(unit +0x2C),
// a null or self target returns 0. Then 0xDAA90(edx = target, esi = unit), 0x91B70(ecx = unit; c), 0xDED30(c, unit, target); owner = 0xAABA(unit) (0 returns 0), n = 0xAD12(owner) - 1
// (below 0 returns 0), 0xAC94(owner, n). With n == 0 the rate m = clamp(0xA42A(target, 0xC, 0) + a1 - 0xA42A(target, 0xC, 0), 1, a1) goes through 0xAC9A(unit, row field, idx, m) -- field +0x144 for
// a type 0 target or one 0xA7F6 accepts, +0x140 when 0xA580(0, target), +0x13C when 0x22A80(eax = target; 8), else the rate is 100 -- and 0x1180(100, unit +0x20) below it sets the flag.
// 0x92BB0 (ebx = idx; c, unit, target, row, a1, flag) and 0xDED30 close it; with n > 0 unit +0xC4 loses bit 0x40 and 0xA35D0(eax = unit; 0x64) runs. Returns 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_95fe0(uint32_t c, uint32_t unit, uint32_t idx, int32_t a1, uint32_t dt, const D2Seam* e)
{
    if ((int32_t)idx < 0 || (int32_t)idx >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    const uint32_t row = idx * 0x23C + d2rd32(dt + 0xB98);
    if (!row) return 0;
    e->call(0xE07A0, unit, c);
    const uint32_t t = e->call(0xA598, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C));
    if (t == unit || !t) return 0;
    e->call(0xDAA90, 0, 0, t, 0, unit);
    e->call(0x91B70, 0, unit, 0, 0, 0, c);
    e->call(0xDED30, 0, 0, 0, 0, 0, c, unit, t);
    const uint32_t own = e->call(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!own) return 0;
    const int32_t n = (int32_t)e->call(0xAD12, 0, 0, 0, 0, 0, own) - 1;
    if (n < 0) return 0;
    e->call(0xAC94, 0, 0, 0, 0, 0, own, (uint32_t)n);
    uint32_t flag = 0;
    if (n <= 0) {
        int32_t rate = 100;
        bool roll = true;
        const int32_t A = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, t, 0xC, 0);
        const int32_t B = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, t, 0xC, 0);
        int32_t m = A + a1 - B;
        if (m <= 1) m = 1;
        if (!(m < a1)) m = a1;
        uint32_t field = 0; bool lvl = true;
        if (!d2rd32(t)) field = d2rd32(row + 0x144);
        else if (e->call(0xA7F6, 0, 0, 0, 0, 0, t)) field = d2rd32(row + 0x144);
        else if (e->call(0xA580, 0, 0, 0, 0, 0, 0, t)) field = d2rd32(row + 0x140);
        else if (e->call(0x22A80, t, 0, 0, 0, 0, 8)) field = d2rd32(row + 0x13C);
        else lvl = false;
        if (lvl) { rate = (int32_t)e->call(0xAC9A, 0, 0, 0, 0, 0, unit, field, idx, (uint32_t)m); roll = rate > 0; }
        if (roll && (int32_t)e->call(0x1180, 100, unit + 0x20) < rate) flag = 1;
    }
    e->call(0x92BB0, 0, 0, 0, idx, 0, c, unit, t, row, (uint32_t)a1, flag);
    e->call(0xDED30, 0, 0, 0, 0, 0, c, unit, t);
    if (n > 0) {
        d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) & 0xFFFFFFBFu);
        e->call(0xA35D0, unit, 0, 0, 0, 0, 0x64);
    }
    return 1;
}
