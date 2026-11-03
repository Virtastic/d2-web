// 1.13c D2Game+0x91F10: stdcall (a0 = unit, a1 = accumulator; ret 8), eax = owner. Same row/level preamble as 0x91A30 (rows through
// stat 0x15E of a0; level = max(stat 0x15F, 0xA400(owner, 0xA3F4(ecx = owner, edx = row, 1))); row word +0x54 = state id below dt+0xBD4, state level 0xA712(a0, state, 0) > 0).
// Then acc+0xC += 0xAC9A(owner, row dword +0x138, row, level) * state level; 0xDF780(eax = owner) feeds ecx of 0xE0BD0 for a unit; with a unit and row word +0x108 in
// 1..dt+0xBC0-1 0xA83E(unit, id, 0) runs. dt = *sgptDataTables.
#include "d2game_seam4e.h"
extern "C" void d2game_91f10(uint32_t owner, uint32_t a0, uint32_t a1, uint32_t dt, const D2Seam* e)
{
    const int32_t idx = (int32_t)e->call(0xA712, 0, 0, 0, 0, 0, a0, 0x15E, 0);
    if (idx < 0 || idx >= (int32_t)d2rd32(dt + 0xBA0)) return;
    const uint32_t row = (uint32_t)idx * 0x23C + d2rd32(dt + 0xB98);
    if (!row) return;
    int32_t v = (int32_t)e->call(0xA712, 0, 0, 0, 0, 0, a0, 0x15F, 0);
    const uint32_t x = e->call(0xA3F4, 0, owner, (uint32_t)idx, 0, 0, 1);
    const int32_t w = (int32_t)e->call(0xA400, 0, 0, 0, 0, 0, owner, x);
    if (w > v) v = w;
    if (v <= 0) return;
    const int32_t st = (int16_t)d2rd16(row + 0x54);
    if (st < 0 || st >= (int32_t)d2rd32(dt + 0xBD4)) return;
    const int32_t lvl = (int32_t)e->call(0xA712, 0, 0, 0, 0, 0, a0, (uint32_t)st, 0);
    if (lvl <= 0) return;
    const uint32_t c = e->call(0xAC9A, 0, 0, 0, 0, 0, owner, d2rd32(row + 0x138), (uint32_t)idx, (uint32_t)v);
    d2wr32(a1 + 0xC, d2rd32(a1 + 0xC) + c * (uint32_t)lvl);
    const uint32_t t = e->call(0xDF780, owner);
    const uint32_t u = e->call(0xE0BD0, 0, t);
    if (!u) return;
    const int32_t id = (int16_t)d2rd16(row + 0x108);
    if (id <= 0 || id >= (int32_t)d2rd32(dt + 0xBC0)) return;
    e->call(0xA83E, 0, 0, 0, 0, 0, u, (uint32_t)id, 0);
}
