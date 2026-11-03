// A5Q4 helpers that spawn object 0x79 beside a unit: 1.13c D2Game+0x5D1D0 (EAX = unit,
// one stack word = game, ret 4) and +0x5D380 ( EAX = unit, stack word = quest record, returns 1 on spawn).
// Every callee is a seam on E: E::h(rva, eax, ecx, edx, ebx, esi, stack words...) and E::rd32/rd8/wr8.
#pragma once
#include "d2game_fatal.h"

namespace d2g5d1d0 {
// A unit position word: unit types 2 and 4..5 keep it in the record at +0x2C, any other asks the engine (#10750 / #10867).
template <class E> uint32_t PosWord(E& e, uint32_t unit, uint32_t off, uint32_t rva)
{
    const int32_t t = (int32_t)e.rd32(unit);
    const uint32_t p = e.rd32(unit + 0x2C);
    if (t == 2 || (t > 3 && t <= 5)) return e.rd32(p + off);
    return p ? e.h(rva, 0, 0, 0, 0, 0, p) : 0;
}
// The shared spawn: creates the object beside `unit`, links it, returns false when the room lookup or the spawn fails.
// Retail pushes #10111's second word from the caller's own stack-argument slot with only its low byte rewritten to
// (#10258 result | 3): that slot holds `game` in 0x5D1D0 and the (x + 5) position word in 0x5D380 (it spilled it there).
template <class E> bool Spawn(E& e, uint32_t unit, uint32_t game, bool slotIsGame, uint32_t mod)
{
    const uint32_t a = PosWord(e, unit, 0x10, 0xA3B8);   // #10750
    const uint32_t b = PosWord(e, unit, 0xC, 0xA454);    // #10867
    const uint32_t owner = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (owner == 0) { d2gfatal::Fatal(e, 0xE42, mod); return false; }
    const uint32_t want = e.h(0xA56E, 0, 0, 0, 0, 0, 0x79) & 0xFF;
    const uint32_t cls = e.h(0xA556, 0, 0, 0, 0, 0, owner);
    if ((e.h(0xA56E, 0, 0, 0, 0, 0, cls) & 0xFF) != want) { d2gfatal::Fatal(e, 0xE5C, mod); return false; }
    const uint32_t y = b + 10, x = a + 5;
    const uint32_t room = e.h(0x1280, owner);
    if (room == 0) return false;
    const uint32_t obj = e.h(0xE1D90, 0, 2, 0x3C, 0, 0, y, x, game, room, 1, 1, 0);
    e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 1);
    const uint32_t r = e.h(0xA22E0, 0, 0, 0, 0, 0, game, obj, 0x79, cls);
    if (r == 0) return false;
    const uint32_t flag = (e.h(0xAEE0, 0, 0, 0, 0, 0, r) & 0xFF) | 3;
    e.h(0xAEC8, 0, 0, 0, 0, 0, r, ((slotIsGame ? game : x) & ~0xFFu) | flag);
    e.h(0xA51A, 0, 0, 0, 0, 0, room, 0);
    e.h(0xA51A, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, r), 0);
    return true;
}
// 1.13c D2Game+0x5D1D0: after the A5Q4 state (type-0x22 quest record in the game's list) is armed (+0x89), a type-4 unit gets the object.
template <class E> void TrySpawnType79(E& e, uint32_t unit, uint32_t game, uint32_t mod)
{
    uint32_t node = 0;
    const uint32_t list = e.rd32(game + 0x10F4);
    if (list) for (node = e.rd32(list); node; node = e.rd32(node + 0xF4)) if (e.rd32(node) == 0x22) break;
    const uint32_t st = e.rd32(node + 0x18);   // retail dereferences a missing node (null): callers always have the 0x22 record
    if (e.rd8(st + 0x89) == 0) return;
    const uint32_t t = e.h(0xA556, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, unit));
    if ((e.h(0xA56E, 0, 0, 0, 0, 0, t) & 0xFF) != 4) return;
    if (Spawn(e, unit, game, true, mod)) e.wr8(st + 0x89, 0);
}
}
