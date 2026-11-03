// 1.13c D2Game+0x3C700: ecx = pGame, edx = missile, [esp+4] unused (ret 4). The missile's record (sgptDataTables +0xB64, stride 0x1A4, count +0xB6C) must
// have (int16)row[0x24] >= 0 and the missile flag bit 10 set; owner = #E03A0(eax = m[0x94], edx = m[0x98], ecx = game). A 23-dword block
// {2, owner, m, ..., (int16)row[0x24] at [4], t1 at [7], t2 at [8], #B0FC(m) at [12]} is handed to #8F930(ecx = game, edx = block) for each offset 0,
// 4*max(row[0x4C],1), ... < 0x100 in the two module tables t1 (0xFC590) and t2 (0xFC490); a unit returned gets #A50E(unit, t1[off]) and #AF5E(unit, t2[off]).
// The first #B26A(m) result lands in the same block dword as #B0FC's and is overwritten (both calls pop their one argument).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_3c700(uint32_t game, uint32_t m, uint32_t dt, const uint8_t* t1, const uint8_t* t2, const D2Seam* e)
{
    if (!m) return 1;
    const int32_t id = (int32_t)d2rd32(m + 4);
    if (id < 0 || id >= (int32_t)d2rd32(dt + 0xB6C)) return 1;
    const uint32_t row = id * 0x1A4 + d2rd32(dt + 0xB64);
    if (!row || (int16_t)d2rd16(row + 0x24) < 0) return 1;
    if (!(d2rd32(m + 0xC8) >> 10 & 1)) return 1;
    const uint32_t owner = e->call(0xE03A0, d2rd32(m + 0x94), game, d2rd32(m + 0x98));
    if (!owner) return 1;
    uint32_t q[23] = {};
    q[0] = 2; q[1] = owner; q[2] = m;
    e->call(0xB26A, 0, 0, 0, 0, 0, m);
    q[12] = e->call(0xB0FC, 0, 0, 0, 0, 0, m);
    q[4] = (uint32_t)(int32_t)(int16_t)d2rd16(row + 0x24);
    int32_t step = (int32_t)d2rd32(row + 0x4C);
    if (step < 1) step = 1;
    step <<= 2;
    for (int32_t off = 0; off < 0x100; off += step) {
        uint32_t a, b; memcpy(&a, t1 + off, 4); memcpy(&b, t2 + off, 4);
        q[7] = a; q[8] = b;
        const uint32_t nu = e->call(0x8F930, 0, game, (uint32_t)(uintptr_t)q);
        e->blk(0x8F930, q, 23);
        if (nu) { e->call(0xA50E, 0, 0, 0, 0, 0, nu, a); e->call(0xAF5E, 0, 0, 0, 0, 0, nu, b); }
    }
    return 3;
}
