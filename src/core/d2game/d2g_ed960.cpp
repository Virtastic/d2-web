// 1.13c D2Game+0xED960 -- spawn a saved monster from its 32-byte record. EAX = record; stack (ret 8): game, s1.
// Record: +0 flags (0x10000 kept), +4, +8 word (added to the level's +0x114, clamped to its +0x116), +0xA word (level id), +0xC.
// Callees: 0xA3BE (level lookup), 0xC1250 (create), 0x96680 (EAX = s1, EBX = unit+0xC, ESI = 7, EDI = &{rec+4, w, rec+0xA}),
// 0xA42A / 0xA48A / 0xA472 / 0x7A3A0 (EBX = 0xD, ESI = unit), 0xA3A6, 0x48BA0, 0x98120, 0xDA750 (EDI = unit), 0xA4A2, 0xAF8D0 (EAX = game, EBX = unit), 0xA448, 0xE20A0 (ECX = game, EDX = unit).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ed960(uint32_t rec, uint32_t game, uint32_t s1, const D2Seam* e)
{
    if (!d2rd32(rec + 0xc) && !d2rd32(rec + 4) && !d2rd16(rec + 8)) return 0;
    uint32_t c[8];
    for (int i = 0; i < 8; ++i) c[i] = d2rd32(rec + 4 * i);
    auto w = [&](uint32_t off) { return (uint32_t)((c[off >> 2] >> ((off & 3) * 8)) & 0xffff); };
    const uint32_t lvl0 = e->call(0xA3BE, 0, 0, 0, 0, 0, d2rd32(game + 0x70), w(0xa), 1);
    if (!lvl0) return 0;
    const uint32_t lo = d2rd16(lvl0 + 0x114), hi = d2rd16(lvl0 + 0x116);
    uint32_t di = (w(8) + lo) & 0xffff;
    if (di > hi) di = lo;
    c[2] = (c[2] & 0xffff0000u) | di;
    const uint32_t flag = c[0] & 0x10000;
    uint32_t dw_a;   // the dword at record+0xA, unaligned
    { uint8_t b[32]; memcpy(b, c, 32); memcpy(&dw_a, b + 0xa, 4); }
    const uint32_t p = e->call(0xC1250, 0, 0, 0, 0, 0, s1, c[2], c[1], dw_a, d2rd32(lvl0 + 8), flag);
    if (!p) return 0;
    uint32_t blk[3] = { c[1], di, w(0xa) };
    e->call(0x96680, s1, 0, 0, d2rd32(p + 0xc), 7);
    e->edi(0x96680, (uint32_t)(uintptr_t)blk);
    e->call(0xA42A, 0, 0, 0, 0, 0, s1, 0xc, 0);
    const uint32_t r1 = e->call(0xA48A, 0, 0, 0, 0, 0, 0);
    const uint32_t cnt = e->call(0xA42A, 0, 0, 0, 0, 0, p, 0xd, 0);
    const uint32_t rec3 = c[3];
    if (cnt < rec3) e->call(0xA472, 0, 0, 0, 0, 0, p, 0xd, rec3, 0);
    e->call(0x7A3A0, 0, 0, 0, 0xd, p, rec3);
    uint32_t lv = e->call(0xA3BE, 0, 0, 0, 0, 0, d2rd32(game + 0x70), w(0xa), 1);
    if (!lv) { e->call(0xE20A0, 0, game, p); return 0; }
    uint32_t counter = 1;
    for (;;) {
        const uint32_t nx = counter + 1;
        if ((int32_t)nx > (int32_t)r1) break;
        if (rec3 < e->call(0xA3A6, 0, 0, 0, 0, 0, nx, d2rd32(lv + 0x20))) break;
        lv = e->call(0xA3BE, 0, 0, 0, 0, 0, d2rd32(game + 0x70), w(0xa), nx);
        counter = nx;
        if (!lv) { e->call(0xE20A0, 0, game, p); return 0; }
    }
    e->call(0x48BA0, 0, 0, 0, 0, 0, game, s1, p, counter);
    if (flag) {
        e->call(0x98120, 0, 0, 0, 0, 0, game, s1, p);
        d2wr32(p + 0xc4, d2rd32(p + 0xc4) | 0x10000);
        e->call(0xDA750, 0, 0, 0, 0, 0, game);
        e->edi(0xDA750, p);
        e->call(0xA4A2, 0, 0, 0, 0, 0, p, 0xc);
        e->call(0xAF8D0, game, 0, 0, p);
    }
    if (!d2rd32(p + 0x60)) e->call(0xA448, 0, 0, 0, 0, 0, 0, p);
    return p;
}
