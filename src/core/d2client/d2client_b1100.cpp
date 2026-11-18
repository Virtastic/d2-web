// 1.13c D2Client+0xB1100: spawn a missile's hit children, ECX = missile unit. Its class (+4) must name a missile record whose word at
// +0x2C is not negative. a = 0xC3FA(unit), b = 0xC388(unit); 0xB0450(that word, 0, 0, b, a) with ESI = unit. When the word at +0x2E is
// not negative a second child is spawned: its id is that word, or when the word at +0x30 is above it a roll 0x1380(ECX = unit+0x20,
// ESI = span) added to it; the unit it returns gets flag 0x10000 at +0xC4. Always 1.
#include "d2c_seam.h"
extern "C" uint32_t d2c_b1100(const uint8_t* mis, int32_t nMissiles, const D2cSeam* s, const uint8_t* unit) {
    if (!unit) return 1;
    int32_t cls; std::memcpy(&cls, unit + 4, 4);
    if (cls < 0 || cls >= nMissiles) return 1;
    const uint8_t* rec = mis + (uint32_t)cls * 0x1a4;
    if (!rec || d2cS16(rec, 0x2c) < 0) return 1;
    const uint64_t U = (uintptr_t)unit;
    uint64_t a = (*s)(0xc3fa, 0, 0, 0, 0, 0, U), b = (*s)(0xc388, 0, 0, 0, 0, 0, U);
    (*s)(0xb0450, 0, 0, 0, 0, U, (uint32_t)(int32_t)d2cS16(rec, 0x2c), 0, 0, b, a);
    const int16_t lo = d2cS16(rec, 0x2e), hi = d2cS16(rec, 0x30);
    if (lo < 0) return 1;
    int32_t id = lo;
    if (hi > lo) {
        const int32_t span = (int32_t)hi - (int32_t)lo + 1;
        id = (int32_t)((uint32_t)(*s)(0x1380, 0, (uintptr_t)(unit + 0x20), 0, 0, (uint32_t)span) + (uint32_t)(int32_t)lo);
    }
    a = (*s)(0xc3fa, 0, 0, 0, 0, 0, U); b = (*s)(0xc388, 0, 0, 0, 0, 0, U);
    uint8_t* child = (uint8_t*)(uintptr_t)(*s)(0xb0450, 0, 0, 0, 0, U, (uint32_t)id, 0, 0, b, a);
    if (child) { uint32_t f; std::memcpy(&f, child + 0xc4, 4); f |= 0x10000; std::memcpy(child + 0xc4, &f, 4); }
    return 1;
}
