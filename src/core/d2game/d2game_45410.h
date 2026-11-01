// 1.13c D2Game+0x45410 (spawn a missile for a skill id and hand it the caster's data), transcribed from the 1.13c disassembly.
// Convention: EAX = skill id, EBX = a value copied twice into the request, stack = (game, caster data pointer or 0, word, word), ret 0x10.
// The caster pointer is also 0x91340's argument; it supplies the first word (default 6) and the word at +0xC (default -1) for the two
// follow-up calls. The 23-dword request (zeroed, then 0x8000, EBX, EBX, r91, id, last stack word, third stack word) goes to 0x6F930 in
// EDX with the game in ECX. The returned value is whatever EAX held at the return: the id, 0, the 0x91340 result or 0xAF5E's result.
#pragma once
#include <cstdint>

namespace r45410 {
template <class E> uint32_t Do(E& e, uint32_t dt, int32_t id, uint32_t ebx, uint32_t game, uint32_t p, uint32_t s2, uint32_t s3)
{
    if (id < 0) return (uint32_t)id;
    if (id >= (int32_t)e.rd32(dt + 0xBA0)) return 0xFFFFFFFFu;   // retail leaves the pinned pointer in EAX here (a host address); not a replayed path
    if (!((uint32_t)id * 0x23C + e.rd32(dt + 0xB98))) return 0;
    const int32_t r91 = (int32_t)e.h(0x91340, (uint32_t)id, 0, 0, 0, 0, p);
    if (r91 <= 0) return (uint32_t)r91;
    uint32_t b[23] = {};
    b[0] = 0x8000;
    b[1] = ebx;
    b[2] = ebx;
    b[4] = (uint32_t)r91;
    b[11] = (uint32_t)id;
    b[12] = s3;
    b[19] = s2;
    const uint32_t m = e.h(0x6F930, 0, game, e.stk(b));
    e.blk(0x6F930, b, 23);
    if (!m) return 0;
    e.h(0xA50E, 0, 0, 0, 0, 0, m, p ? e.rd32(p) : 6);
    return e.h(0xAF5E, 0, 0, 0, 0, 0, m, p ? e.rd32(p + 0xC) : 0xFFFFFFFFu);
}
}
