// D2 seed / PRNG (D2Common 1.13c: SEED_* exports #10912-10922 plus the inlined
// SEED_RollRandomNumber @0x6FD78E30 and SEED_RollLimitedRandomNumber @0x6FD7D3E0).
// Ground truth: the retail captures (s01-prng).
//
// Everything here is integer: the retail code is a 64-bit LCG step with no float anywhere,
// so wasm reproduces it bit-for-bit. Types are exact-width on purpose.
#pragma once
#include <stdint.h>

namespace d2 {

#pragma pack(push, 1)
struct Seed {
    uint32_t low;    // +0
    uint32_t high;   // +4   -- retail reads/writes the pair as one u64
};
#pragma pack(pop)
static_assert(sizeof(Seed) == 8, "D2SeedStrc is 8 bytes");

inline void seedInit(Seed& s)                    { s.low = 1;   s.high = 666; }   // SEED_InitSeed #10912
inline void seedInitLow(Seed& s, uint32_t low)   { s.low = low; s.high = 666; }   // SEED_InitLowSeed #10913

// SEED_RollRandomNumber: state' = high + 0x6AC690C5 * low, as one 64-bit value written back
// over {low, high}. Returns the full 64 bits; callers take the low 32.
inline uint64_t roll(Seed& s) {
    uint64_t v = (uint64_t)s.high + 0x6AC690C5ull * (uint64_t)s.low;
    s.low  = (uint32_t)v;
    s.high = (uint32_t)(v >> 32);
    return v;
}

// SEED_RollLimitedRandomNumber: power-of-two nMax masks, anything else takes the low 32 bits
// modulo nMax (unsigned). nMax <= 0 rolls NOTHING and returns 0 -- the seed is untouched.
inline uint32_t rollLimited(Seed& s, int32_t nMax) {
    if (nMax <= 0) return 0;
    if ((nMax - 1) & nMax) return (uint32_t)roll(s) % (uint32_t)nMax;
    return (uint32_t)(roll(s) & (uint64_t)(uint32_t)(nMax - 1));
}

inline uint32_t rollPercentage(Seed& s) { return (uint32_t)roll(s) % 100u; }

} // namespace d2
