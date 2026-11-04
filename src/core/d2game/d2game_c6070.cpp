// D2Game+0xC6070 -- ECX = game, EDX = unit, one unused stack word (ret 4). Written from the 1.13c disassembly; callees are the D2Seam.
// A478(unit) must be non-zero. The quest list game+0x10F4 (head pointer, nodes through +0xF4, kind at +0) is walked to the kind-8 node; its data
// block (node+0x18) word +4 is compared with A478(unit), else A55C(second A478(unit), &array, &count) fills an array that is searched for that word.
// A hit sets a flag; A4C0(unit) gives a table indexed by the byte game+0x6D; two A568 checks (non-zero ends), a clear flag ends, then three A57A
// calls, 34C00(EAX = unit, stack node+4), DFA60(EAX = unit) (low word 0xFFFF ends), 9C160(EAX = DFA60 result, ESI = game, EBX = 0, stack callback 0xC55B0).
// The unreachable fatal paths (no list / no kind-8 node / second A478 zero) are the error report plus exit in retail; they return here.
#include "d2game_seam4e.h"
// 1.13c D2Game+0xC6070
extern "C" void d2game_c6070(uint32_t game, uint32_t unit, const D2Seam* e)
{
    const uint32_t a = e->call(0xA478, 0, 0, 0, 0, 0, unit);
    if (!a) return;
    const uint32_t list = d2rd32(game + 0x10F4);
    if (!list) return;
    uint32_t n = d2rd32(list);
    if (!n) return;
    while (d2rd32(n) != 8) { n = d2rd32(n + 0xF4); if (!n) return; }
    const uint32_t d = d2rd32(n + 0x18);
    uint32_t esi = 0;
    if (a == d2rd32(d + 4)) esi = 1;
    else {
        const uint32_t b = e->call(0xA478, 0, 0, 0, 0, 0, unit);
        if (!b) return;
        volatile uint32_t arr = 0, cnt = 0;
        e->call(0xA55C, 0, 0, 0, 0, 0, b, (uint32_t)(uintptr_t)&arr, (uint32_t)(uintptr_t)&cnt);
        const uint32_t key = d2rd32(d + 4);
        for (uint32_t i = 0; i < cnt; i++) if (d2rd32(arr + 4 * i) == key) { esi = 1; break; }
    }
    const uint32_t tbl = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t h = d2rd32(tbl + 4 * d2rd8(game + 0x6D) + 0x10);
    if (e->call(0xA568, 0, 0, 0, 0, 0, h, d2rd32(n + 0xE0), 0)) return;
    if (e->call(0xA568, 0, 0, 0, 0, 0, h, d2rd32(n + 0xE0), 1)) return;
    if (!esi) return;
    e->call(0xA57A, 0, 0, 0, 0, 0, h, d2rd32(n + 0xE0), 0xD);
    e->call(0xA57A, 0, 0, 0, 0, 0, h, d2rd32(n + 0xE0), 1);
    e->call(0xA57A, 0, 0, 0, 0, 0, h, 9, 5);
    e->call(0x34C00, unit, 0, 0, 0, 0, d2rd32(n + 4));
    const uint32_t r = e->call(0xDFA60, unit);
    if ((r & 0xFFFFu) == 0xFFFFu) return;
    e->call(0x9C160, r, 0, 0, 0, game, 0xB2000000u | 0xC55B0);
}
