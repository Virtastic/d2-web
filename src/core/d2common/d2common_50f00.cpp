// 1.13c D2Common+0x50F00 -- do the two hand items satisfy the skill's item-type requirement (EAX = skill, stack unit, ret 4).
// Written from the disassembly; callees behind an env so the FN probe replays retail's hooked sequence.
// Offsets (1.13c, 32-bit): skill +0 skills.txt record; record +0 (s16) skill class word, +0x18 / +0x24 (s16) item-type counts;
// unit +0x60 inventory; inventory +0 signature 0x01020304; grid slot +0x0C equipment, equipment +0x10 / +0x14 the two hands.
#include <cstdint>
#include <cstring>
#include "d2common_50f00.h"

namespace {
inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 4); return v; }
inline int16_t rd16(uint32_t a) { int16_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 2); return v; }
}

// 1.13c D2Common+0x50F00
extern "C" uint32_t D2Common_50F00_SkillItemTypes(uint32_t skill, uint32_t unit, const D2Common50F00Env* e)
{
    if (!skill) return 0;
    const uint32_t txt = rd32(skill);
    if (!txt) return 0;
    if (rd16(txt + 0x24) <= 0 && rd16(txt + 0x18) <= 0) return 1;
    const uint32_t inv = rd32(unit + 0x60);
    if (!inv) return 0;
    uint32_t a = 0, b = 0;
    if (rd32(inv) == 0x01020304u) { const uint32_t s = e->gridSlot(inv); a = s ? rd32(rd32(s + 0x0C) + 0x10) : 0; }
    if (rd32(inv) == 0x01020304u) { const uint32_t s = e->gridSlot(inv); b = s ? rd32(rd32(s + 0x0C) + 0x14) : 0; }
    const int16_t cls = rd16(txt);
    if (cls == 4 || cls == 5) {
        const uint32_t w = e->weapon(inv);
        if (w == a) a = 0;
        else if (w == b) b = 0;
    }
    if (e->fits(0, a, b, txt)) return e->fits(1, b, a, txt);
    if (!e->fits(1, a, b, txt)) return 0;
    return e->fits(0, b, a, txt);
}
