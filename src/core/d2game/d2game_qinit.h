// Shared shape of four 1.13c D2Game quest-record init callbacks (table entries 0x5A7C0, 0x63CC0, 0xC5730,
// 0xE6700). ecx = the quest record. The fifteen callback words at +0xA0 are cleared, then: +0xA0 and +0xCC take
// two handlers, +0xDC the quest's message table, +0xE0 the count 0x2A, +0xE8 and +0xEC two more handlers; the
// byte at +0x0A becomes 1, the bytes at +0x0B and +0x0C and the word at +0x18 become 0. The result is 0.
// A module address is spelled 0xB2000000 | rva, as the handler tables of this directory spell theirs.
#pragma once
#include <cstdint>

namespace gqinit {
struct Words { uint32_t a0, cc, dc, e8, ec; };
template <class E> uint32_t Init(E& e, uint32_t q, const Words& w)
{
    const uint32_t kMod = 0xB2000000u;
    for (uint32_t i = 0; i < 15; i++) e.wr32(q + 0xA0 + 4 * i, 0);
    e.wr32(q + 0xA0, kMod | w.a0);
    e.wr32(q + 0xCC, kMod | w.cc);
    e.wr32(q + 0xDC, kMod | w.dc);
    e.wr8(q + 0x0A, 1);
    e.wr8(q + 0x0C, 0);
    e.wr8(q + 0x0B, 0);
    e.wr32(q + 0x18, 0);
    e.wr32(q + 0xE0, 0x2A);
    e.wr32(q + 0xE8, kMod | w.e8);
    e.wr32(q + 0xEC, kMod | w.ec);
    return 0;
}
}
