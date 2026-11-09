// 1.13c D2Game+0xECF30: corpse section ('jM') reader, stdcall (ret 0x18): game, player, &cursor, end, type, &errOut.
// Each corpse unit is allocated (0xA6B8), set up, then 0xECB50 reads its items (EAX = end, EDX = type, EBX = &cursor,
// EDI = unit, stack game, &local).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ecf30(uint32_t game, uint32_t player, uint32_t ppCur, uint32_t end, uint32_t type, uint32_t err, const D2Seam* e)
{
    auto fail = [&](uint32_t code) { if (err) d2wr32(err, code); return 0x15u; };
    uint32_t cur = d2rd32(ppCur), cfg = 0;
    if (!player || d2rd32(player) != 0) return fail(0x65);
    const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, player);
    cfg = d2rd32(pd + 0x9c);
    if (!cfg) return fail(0x65);
    if (end < cur + 4) return fail(0x66);
    if (d2rd16(cur) != 0x4d4a) return fail(0x67);
    const uint32_t count = d2rd16(cur + 2);
    cur += 4;
    if (count > 1) return fail(0x68);
    for (int32_t i = 0; i < (int32_t)count; ++i) {
        if (cur + 0xc > end) return fail(0x69);
        uint32_t curVar = cur + 0xc;
        const uint32_t cls = d2rd32(player + 4);
        if (cls >= 7) return fail(0x6a);
        const uint32_t gid = d2rd32(game + 0x1c);
        const uint32_t u = e->call(0xA6B8, 0, 0, 0, 0, 0, gid, 0);
        if (!u) return fail(0x6a);
        d2wr32(u, 0);
        d2wr32(u + 4, cls);
        d2wr32(u + 8, gid);
        d2wr32(u + 0x80, game);
        uint32_t c8 = d2rd32(u + 0xc8);
        c8 = d2rd32(game + 0x70) ? (c8 | 0x2000000) : (c8 & 0xfdffffffu);
        d2wr32(u + 0xc8, c8);
        d2wr32(u + 0xc8, c8 | 0x4000000);
        const uint32_t a = e->call(0xA556, 0, 0, 0, 0, 0, 0);
        const uint32_t act = e->call(0xA56E, 0, 0, 0, 0, 0, a) & 0xff;
        d2wr8(u + 0x18, act);
        d2wr32(u + 0x1c, d2rd32(game + 0xbc + act * 4));
        d2wr32(u + 0x74, 0);
        d2wr32(u + 0xc4, d2rd32(u + 0xc4) | 0x10);
        d2wr32(u + 0xd0, 0xb);
        uint32_t ctr = d2rd32(game + 0x90) + 1;
        if (ctr == 0xffffffffu) ctr = 1;
        d2wr32(game + 0x90, ctr);
        e->call(0x38620, game, 0, 0, 0, u, ctr);
        e->call(0xE0F40, 0, 0, u, 0, 0, 0, game, 1);
        const uint32_t mode = d2rd32(u + 0x10);
        if (mode == 0x11 || mode == 0) {
            e->call(0xA6AC, 0, 0, 0, 0, 0, u, 1);
            e->call(0xA6D6, 0, 0, 0, 0, 0, u, 5);
            e->call(0xA682, 0, 0, 0, 0, 0, d2rd32(u + 0x2c), 0x8000);
        }
        d2wr32(u + 0xc4, d2rd32(u + 0xc4) & ~4u);
        e->call(0xE0040, u, 0, 0, 0, 0, cfg);
        const uint32_t r49 = e->call(0x49A20, cfg);
        e->call(0xA40C, 0, 0, 0, 0, 0, u, r49);
        e->call(0xE0110, 0, 0, 0, 0, game + 0xd0);
        e->edi(0xE0110, u);
        e->call(0xA3CA, 0, 0, 0, 0, 0, u, 7, 1);
        d2wr32(u + 0xc4, d2rd32(u + 0xc4) & ~0x10u);
        e->call(0xA39A, 0, 0, 0, 0, 0, d2rd32(u + 0x60), d2rd32(player + 0xc));
        uint32_t local = 0;
        const uint32_t r = e->call(0xECB50, end, 0, type, (uint32_t)(uintptr_t)&curVar, 0, game, (uint32_t)(uintptr_t)&local);
        e->edi(0xECB50, u);
        if (r) return fail(local);
        e->call(0xA424, 0, 0, 0, 0, 0, d2rd32(player + 0x60), d2rd32(u + 0xc), 0, 1);
        e->call(0x8CF80, d2rd32(player + 0xc), d2rd32(u + 0xc), 0, 0, 0, 1);
        e->edi(0x8CF80, game);
        cur = curVar;
    }
    d2wr32(ppCur, cur);
    return 0;
}
