// 1.13c D2Client+0xBB1B0: ECX = missile unit. When its class (+4) names a missile record whose word at +0x2C is not negative, spawns
// the hit ring through 0xB6BC0 (EAX = unit, EBX = that word, ECX = record +0x6C, EDX = +0x74; stack +0x6C, +0x70). Always 1.
#include "d2c_seam.h"
extern "C" uint32_t d2c_bb1b0(const uint8_t* mis, int32_t nMissiles, const D2cSeam* s, const uint8_t* unit) {
    if (!unit) return 1;
    int32_t cls; std::memcpy(&cls, unit + 4, 4);
    if (cls < 0 || cls >= nMissiles || !mis) return 1;
    const uint8_t* rec = mis + (uint32_t)cls * 0x1a4;
    int32_t w = d2cS16(rec, 0x2c);
    if (w < 0) return 1;
    uint32_t a, b, c; std::memcpy(&a, rec + 0x6c, 4); std::memcpy(&b, rec + 0x70, 4); std::memcpy(&c, rec + 0x74, 4);
    (*s)(0xb6bc0, (uintptr_t)unit, a, c, (uint64_t)(int64_t)w, 0, a, b);
    return 1;
}
