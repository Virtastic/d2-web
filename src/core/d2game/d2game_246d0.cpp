// 1.13c D2Game+0x246D0: eax = class/mode word, ecx = pGame, stack a0 (room/act arg), a1, a2 (x, y), a3, a4 = pointer to 9 unique-mod bytes (ret 0x14). Builds a 10-dword spawn request
// {a0, ecx, 0, eax, 1, a3, a1, a2, -1, 0x62} and calls #EEF10(ecx = &request); on failure the same request with mode word 4; on a second failure
// #47450(a0, ecx, 0, eax, &a1, &a2, 1) relocates the point (it writes a1/a2 through the pointers; 0 returns 0) and #EF450(eax = a3, ecx = eax_in, edx = 1; a0, ecx, a1, a2, -1, 0x62)
// spawns (null returns 0). The unit's monster data (unit[0x14], type word unit[0] == 1) gets the 9 bytes copied to data+0x1C, data[0x16] |= 0x10, then the handler table
// (retail module table of function pointers, indexed by mod number) runs for mods 1..4 and then each of the 9 bytes: a non-null entry is called with ecx = unit and one 0 stack word.
// The unused high half of request word 9 is stack residue in retail (its hookblk keeps 38 bytes). Returns the unit.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_246d0(uint32_t eaxIn, uint32_t game, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t mods, uint32_t tbl, const D2Seam* e)
{
    uint32_t q[10] = { a0, game, 0, eaxIn, 1, a3, a1, a2, 0xFFFFFFFFu, 0x62 };
    uint32_t u = e->call(0xEEF10, 0, (uint32_t)(uintptr_t)q);
    e->blk(0xEEF10, q, 10);
    if (!u) {
        q[8] = 4; q[2] = u;
        u = e->call(0xEEF10, 0, (uint32_t)(uintptr_t)q);
        e->blk(0xEEF10, q, 10);
        if (!u) {
            uint32_t x = a1, y = a2;
            if (!e->call(0x47450, 0, 0, 0, 0, 0, a0, game, u, eaxIn, (uint32_t)(uintptr_t)&x, (uint32_t)(uintptr_t)&y, 1)) return 0;
            u = e->call(0xEF450, a3, eaxIn, 1, 0, 0, a0, game, x, y, 0xFFFFFFFFu, 0x62);
            if (!u) return 0;
        }
    }
    uint32_t data = 0;
    if (d2rd32(u) == 1) data = d2rd32(u + 0x14);
    const uint32_t ed = data ? data + 0x1C : 0;
    for (int i = 0; i < 9; i++) d2wr8(ed + i, d2rd8(mods + i));
    if (d2rd32(u) == 1 && d2rd32(u + 0x14)) d2wr8(d2rd32(u + 0x14) + 0x16, d2rd8(d2rd32(u + 0x14) + 0x16) | 0x10);
    for (uint32_t k = 1; k <= 4; k++) { const uint32_t f = d2rd32(tbl + 4 * k); if (f) e->call(f & 0xFFFFFF, 0, u, 0, 0, 0, 0); }
    for (int i = 0; i < 9; i++) { const uint32_t f = d2rd32(tbl + 4 * d2rd8(ed + i)); if (f) e->call(f & 0xFFFFFF, 0, u, 0, 0, 0, 0); }
    return u;
}
