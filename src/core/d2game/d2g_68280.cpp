// 1.13c D2Game+0x68280 -- stdcall (cb, byte a1, byte a2), ret 0xC, no result. Written from the 1.13c disassembly.
// data = cb+0x18, game = cb+4 (null asserts, line 0x2D3; not driven). +0x48 == 1: the unit with id data+0x34 (game hash
// +0x1520, chain +0xE4) of class 0x1A not already in mode 3 is set to mode 3; data+0x54 = 3 unless that skip was taken.
// +0x47 == 1: the unit with id data+0x30 gets mode 1. a1: first time (+0x46) 0x33AF0(cb, 0x68250) with EBX 1; else a2:
// 0xDFCC0(EAX 1, EBX 0x68150, EDI game; 0). a2 also: +0x61 clear calls 0x1EC40(EBX game, AL 0, ECX 0x92); +0x51 clear sets
// +0x52. Finally +0x50 = 1.
#include "../fn/hookenv.h"
#include "d2game_fatal.h"
namespace d2g {
static uint32_t find68280(HookEnv& e, uint32_t game, uint32_t id)
{
    uint32_t u = e.rd32(game + 0x1520 + (id & 0x7F) * 4);
    while (u && e.rd32(u + 0xC) != id) u = e.rd32(u + 0xE4);
    return u;
}
void f68280(HookEnv& e, uint32_t cb, uint32_t a1, uint32_t a2)
{
    const uint32_t d = e.rd32(cb + 0x18);
    a1 &= 0xFF; a2 &= 0xFF;
    bool set54 = true;
    if (e.rd8(d + 0x48) == 1) {
        const uint32_t game = e.rd32(cb + 4);
        if (!game) { d2gfatal::Fatal(e, 0x2D3, 0xB2000000u); return; }
        const uint32_t u = find68280(e, game, e.rd32(d + 0x34));
        if (u && e.rd32(u + 4) == 0x1A) {
            if (e.rd32(u + 0x10) == 3) set54 = false;
            else e.h(0xA4A2, 0, 0, 0, 0, 0, u, 3);
        }
    }
    if (set54) e.wr32(d + 0x54, 3);
    if (e.rd8(d + 0x47) == 1) {
        const uint32_t game = e.rd32(cb + 4);
        if (!game) { d2gfatal::Fatal(e, 0x2D3, 0xB2000000u); return; }
        const uint32_t u = find68280(e, game, e.rd32(d + 0x30));
        if (u) e.h(0xA4A2, 0, 0, 0, 0, 0, u, 1);
    }
    if (a1) {
        if (!e.rd8(d + 0x46)) {
            e.wr8(d + 0x46, 1);
            e.h(0x33AF0, 0, 0, 0, 1, 0, cb, 0xB2000000u | 0x68250);
        }
    } else if (a2) {
        const uint32_t game = e.rd32(cb + 4);
        e.h(0xDFCC0, 1, 0, 0, 0xB2000000u | 0x68150, 0, 0);
        e.edi(0xDFCC0, game);
    }
    if (a2) {
        if (!e.rd8(d + 0x61)) e.h(0x1EC40, 0, 0x92, 0, e.rd32(cb + 4));
        if (!e.rd8(d + 0x51)) e.wr8(d + 0x52, 1);
    }
    e.wr8(d + 0x50, 1);
}
}
