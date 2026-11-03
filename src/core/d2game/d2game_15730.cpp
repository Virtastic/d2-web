// Generic spawn pick (1.13c D2Game+0x15730, called from 0x15E00). ECX = the spawning unit (type 1 uses pData +0x58 as the region slot),
// stack = game (region pointers at +0xF0 + slot * 4), stack = out id; ret 8, always returns 1.
// The default id is 0x1C5 when the monster table count (sgptDataTables +0xA80) is above 0x1C5, else -1. A region (byte +0x10 = entry count,
// entries 0x34 apart with a signed short monster id at +0x14) is rolled with the unit seed (+0x20 lo, +0x24 hi, lo * 0x6AC690C5 + hi, new low
// word masked when the count is a power of two, else taken modulo the count); the chosen id wins when it is in range and the 0x15D0 flag
// test (monster id, flag 0x15) passes. Otherwise the first entry whose monster record (+0xA78 base, 0x1A8 stride) has a bit of gdwBitMasks[5]
// in the byte at +0xE wins; failing that the id is 0x1F0 when the count is above 0x1F0, else -1.
// Retail is 32-bit: dt is the sgptDataTables struct, bitMasks the gdwBitMasks array, flagTest the 0x15D0 seam (EAX = id, EDX = flag).
#include <cstdint>
#include <cstring>

struct D2G15730Env {
    const uint8_t* dt;                                 // *sgptDataTables
    const uint32_t* bitMasks;                          // gdwBitMasks
    int32_t (*flagTest)(uint32_t id, uint32_t flag);   // 0x15D0
};

namespace {
inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline int16_t rd16s(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
inline const uint8_t* ptr(uint32_t v) { return (const uint8_t*)(uintptr_t)v; }
}

// 1.13c D2Game+0x15730
extern "C" uint32_t d2g_15730(uint8_t* unit, const uint8_t* game, uint32_t* out, const D2G15730Env* env) {
    const int32_t count = (int32_t)rd32(env->dt, 0xA80);
    *out = count > 0x1C5 ? 0x1C5u : 0xFFFFFFFFu;
    uint32_t slot = 0;
    if (unit && rd32(unit, 0) == 1 && rd32(unit, 0x14)) slot = rd32(ptr(rd32(unit, 0x14)), 0x58);
    const uint8_t* region = ptr(rd32(game, 0xF0 + slot * 4));
    const uint32_t n = region[0x10];
    uint32_t roll = 0;
    if (n) {
        const uint64_t x = (uint64_t)rd32(unit, 0x20) * 0x6AC690C5u + rd32(unit, 0x24);
        const uint32_t lo = (uint32_t)x, hi = (uint32_t)(x >> 32);
        std::memcpy(unit + 0x24, &hi, 4); std::memcpy(unit + 0x20, &lo, 4);
        roll = (n & (n - 1)) == 0 ? (lo & (n - 1)) : lo % n;
    }
    int32_t id = rd16s(region, roll * 0x34 + 0x14);
    if (id >= 0 && id < count && env->flagTest((uint32_t)id, 0x15)) { *out = (uint32_t)id; return 1; }
    for (uint32_t i = 0; i < n; i++) {
        id = rd16s(region, 0x14 + i * 0x34);
        if (id < 0 || id >= count) continue;
        const uint8_t* rec = ptr(rd32(env->dt, 0xA78)) + (uint32_t)id * 0x1A8u;
        if (rec && (rec[0xE] & env->bitMasks[5])) { *out = (uint32_t)id; return 1; }
    }
    *out = count > 0x1F0 ? 0x1F0u : 0xFFFFFFFFu;
    return 1;
}
