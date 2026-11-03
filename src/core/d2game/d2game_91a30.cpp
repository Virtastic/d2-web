// 1.13c D2Game+0x91A30: stdcall (a0 = holder unit, a1 = owner unit, a2 = accumulator; ret 0xC).
// The owner's stat 0x15E picks a row of the skills table (dt+0xB98, stride 0x23C, count dt+0xBA0), stat 0x15F its level, raised by 0xA400(a0, 0xA3F4(ecx = a0,
// edx = row, 1)); a level above 0 and row word +0x54 (a state id below dt+0xBD4) read back as the state level through 0xA712(a1, state, 0); then 0x9F210
// (eax = ebx = row, stack a2, level) runs. A row whose byte +0x1DC is 4 with state level 2 or 3 adds acc+0x30 / 0x21BC0(eax = row) to acc+0x34 (when it is nonzero);
// either way acc+5 |= 0x40. dt = *sgptDataTables.
#include "d2game_seam4e.h"
extern "C" void d2game_91a30(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t dt, const D2Seam* e)
{
    const int32_t idx = (int32_t)e->call(0xA712, 0, 0, 0, 0, 0, a1, 0x15E, 0);
    if (idx < 0 || idx >= (int32_t)d2rd32(dt + 0xBA0)) return;
    const uint32_t row = (uint32_t)idx * 0x23C + d2rd32(dt + 0xB98);
    if (!row) return;
    int32_t v = (int32_t)e->call(0xA712, 0, 0, 0, 0, 0, a1, 0x15F, 0);
    const uint32_t x = e->call(0xA3F4, 0, a0, (uint32_t)idx, 0, 0, 1);
    const int32_t w = (int32_t)e->call(0xA400, 0, 0, 0, 0, 0, a0, x);
    if (w > v) v = w;
    if (v <= 0) return;
    const int32_t st = (int16_t)d2rd16(row + 0x54);
    if (st < 0 || st >= (int32_t)d2rd32(dt + 0xBD4)) return;
    const int32_t lvl = (int32_t)e->call(0xA712, 0, 0, 0, 0, 0, a1, (uint32_t)st, 0);
    if (lvl <= 0) return;
    e->call(0x9F210, (uint32_t)idx, 0, 0, (uint32_t)idx, 0, a2, (uint32_t)v);
    if (d2rd8(row + 0x1DC) == 4 && lvl > 1 && lvl < 4) {
        const int32_t d = (int32_t)e->call(0x21BC0, (uint32_t)idx);
        if (d != 0) {
            const int32_t q = (int32_t)d2rd32(a2 + 0x30) / d;
            d2wr32(a2 + 0x34, d2rd32(a2 + 0x34) + (uint32_t)q);
            d2wr8(a2 + 5, d2rd8(a2 + 5) | 0x40);
            return;
        }
    }
    d2wr8(a2 + 5, d2rd8(a2 + 5) | 0x40);
}
