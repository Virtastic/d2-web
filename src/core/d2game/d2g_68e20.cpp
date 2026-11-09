// 1.13c D2Game+0x68E20 -- stdcall (cb, unit), ret 8, no result. Written from the 1.13c disassembly.
// data = cb+0x18. First call (+0x90 clear): set it, count = 0x1780(ECX quests+0x18, EAX 0x14, EDI 0xA) into +0x8C and tag
// the unit (+0xB8 = 'gld '). A null quest list asserts (line 0x1FB; not driven). While +0x8C: 0x11A70 (EBX game; unit, 2,
// &zero, 0, -1, 0) nonzero decrements it, and a count still > 0 posts event 7 at frame + 10 (0x8EA80).
#include "../fn/hookenv.h"
#include "d2game_fatal.h"
namespace d2g {
void f68e20(HookEnv& e, uint32_t cb, uint32_t unit)
{
    const uint32_t d = e.rd32(cb + 0x18);
    if (!e.rd8(d + 0x90)) {
        e.wr8(d + 0x90, 1);
        const uint32_t qs = e.rd32(e.rd32(cb + 4) + 0x10F4);
        if (!qs) { d2gfatal::Fatal(e, 0x1FB, 0xB2000000u); return; }
        const uint32_t v = e.h(0x1780, 0x14, qs + 0x18);
        e.edi(0x1780, 0xA);
        e.wr32(d + 0x8C, v);
        e.wr32(unit + 0xB8, 0x20646C67);
    }
    if (!e.rd32(d + 0x8C)) return;
    uint32_t zero = 0;
    const uint32_t game = e.rd32(cb + 4);
    if (!e.h(0x11A70, 0, 0, 0, game, 0, unit, 2, e.stk(&zero), 0, 0xFFFFFFFFu, 0)) return;
    const uint32_t c = e.rd32(d + 0x8C) - 1;
    e.wr32(d + 0x8C, c);
    if ((int32_t)c <= 0) return;
    e.h(0x8EA80, 0, e.rd32(cb + 4), unit, 0, 0, 7, e.rd32(e.rd32(cb + 4) + 0xA8) + 10, 0, 0, 0);
}
}
