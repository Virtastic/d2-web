// D2Game+0xD7E30 -- belt boxes shrink: unequip the belt slots past the new box count. EAX = belt item (or null), stack a0 game, a1 unit (ret 8).
// Written from the 1.13c disassembly. inv = [unit + 0x60] (zero: assert exit, not modelled). rec[0x120]: EAX != 0: 0xA8B6(0xA892(EAX), 0, rec), else
// 0xA8B6(2, 0, rec); nBoxes = (u8) rec[4]. For i in 0..15: it = 0xA8E6(inv, i); i < nBoxes (signed) or it == 0 skips. Else local = 0, 0xAA5A(&local, 0x20, 1);
// ebx = [unit] == 0 ? [0xA4C0(unit) + 0x9C] : 0; 0x8D3D0 (EBX = ebx, ESI = it, stack 0xF, local, 0); r = 0xA748(inv, it) (r == 0 or r != it: assert exit,
// not modelled); v = type 2 or 4..5 (signed) ? [[it + 0x2C] + 0xC] : [it + 0x2C] ? 0xA454([it + 0x2C]) : 0; 0xAA7E(inv, v); 0xA4A2(r, 4);
// 0xD4630 (EAX = it, ECX = unit); 0xA92E(it, 0); 0xD76A0 (ECX = file, EDX = 0xC2A, stack game, unit, [it + 0xC], 0, 0, 1, 1, 0). Zero result:
// 0xA544(unit, &a), room = 0xA478(unit), room2 = 0xE930 (ESI = &a, stack room, &b, 1); room2 != 0: 0xD46B0 (ESI = unit, stack game, 0, 1),
// 0xE990 (EAX = b[1], ECX = it, EBX = room2, stack game, unit, b[0]).
#include "d2game_seam4e.h"
static const uint32_t kFile = 0xB20FB14Cu;   // the source-file string at D2Game+0xFB14C, recorded as a module address
// 1.13c D2Game+0xD7E30
extern "C" void d2game_d7e30(uint32_t belt, uint32_t game, uint32_t unit, const D2Seam* e)
{
    const uint32_t inv = d2rd32(unit + 0x60);
    if (!inv) return;
    uint8_t rec[0x120] = {};
    const uint32_t recp = (uint32_t)(uintptr_t)rec;
    if (belt) e->call(0xA8B6, 0, 0, 0, 0, 0, e->call(0xA892, 0, 0, 0, 0, 0, belt), 0, recp);
    else e->call(0xA8B6, 0, 0, 0, 0, 0, 2, 0, recp);
    const uint32_t nBoxes = rec[4];
    for (int32_t i = 0; i < 16; ++i) {
        const uint32_t it = e->call(0xA8E6, 0, 0, 0, 0, 0, inv, (uint32_t)i);
        if (i < (int32_t)nBoxes || !it) continue;
        uint32_t local = 0;
        e->call(0xAA5A, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)&local, 0x20, 1);
        uint32_t ebx = 0;
        if (d2rd32(unit) == 0) ebx = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C);
        e->call(0x8D3D0, 0, 0, 0, ebx, it, 0xF, local, 0);
        const uint32_t r = e->call(0xA748, 0, 0, 0, 0, 0, inv, it);
        if (!r || r != it) return;
        const int32_t type = (int32_t)d2rd32(it);
        const uint32_t p = d2rd32(it + 0x2C);
        uint32_t v;
        if (type == 2 || (type > 3 && type <= 5)) v = d2rd32(p + 0xC);
        else v = p ? e->call(0xA454, 0, 0, 0, 0, 0, p) : 0;
        e->call(0xAA7E, 0, 0, 0, 0, 0, inv, v);
        e->call(0xA4A2, 0, 0, 0, 0, 0, r, 4);
        e->call(0xD4630, it, unit);
        e->call(0xA92E, 0, 0, 0, 0, 0, it, 0);
        if (e->call(0xD76A0, 0, kFile, 0xC2A, 0, 0, game, unit, d2rd32(it + 0xC), 0, 0, 1, 1, 0)) continue;
        uint32_t bufA[2] = {}, bufB[2] = {};
        e->call(0xA544, 0, 0, 0, 0, 0, unit, (uint32_t)(uintptr_t)bufA);
        const uint32_t room = e->call(0xA478, 0, 0, 0, 0, 0, unit);
        const uint32_t room2 = e->call(0xE930, 0, 0, 0, 0, (uint32_t)(uintptr_t)bufA, room, (uint32_t)(uintptr_t)bufB, 1);
        if (!room2) continue;
        e->call(0xD46B0, 0, 0, 0, 0, unit, game, 0, 1);
        e->call(0xE990, bufB[1], it, 0, room2, 0, game, unit, bufB[0]);
    }
}
