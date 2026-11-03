// Shared body of D2Common 0x19190 and 0x19650 (1.13c): load two 0x34-byte-row token tables from the archive in EAX, pack them into one
// pool block (first table, then second), free both loader results and publish (total, pool, pool, pool + first*0x34) into four module globals.
// Seams are retail's real callees: load = 0x5EF40 (5 stack words: archive, name, desc, &count, 0x34; returns the rows),
// alloc = Fog alloc 0x9222 (EDX = size; stack file, line, 0), freeFn = Fog free 0x9228 (EDX = block, minus 4 when the module flag at 0x99E20 is set; stack file, line, 0).
#pragma once
#include <cstdint>
#include <cstring>

struct D2CommonTokPairEnv {
    uint32_t (*load)(uint32_t arch, uint32_t nameRva, uint32_t* count);
    uint32_t (*alloc)(uint32_t line, uint32_t size);
    void (*freeFn)(uint32_t block, uint32_t line);   // block already adjusted, line already chosen (0x8F6 / 0x8F8)
    void (*fatal)(uint32_t code);
    uint32_t flag;                                   // dword at module 0x99E20
};
struct D2CommonTokPairOut { uint32_t total, pool, pool2, second; };

inline D2CommonTokPairOut d2common_tokpair(uint32_t arch, uint32_t nameA, uint32_t nameB, uint32_t allocLine, const D2CommonTokPairEnv* E) {
    uint32_t cntA = 0, cntB = 0;
    const uint32_t a = E->load(arch, nameA, &cntA);
    const uint32_t b = E->load(arch, nameB, &cntB);
    const uint32_t total = cntA + cntB;
    const uint32_t pool = E->alloc(allocLine, total * 0x34);
    if (!pool) { E->fatal(allocLine + 1); return {}; }
    memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(pool)), reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), cntA * 0x34);
    const uint32_t second = pool + cntA * 0x34;
    memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(second)), reinterpret_cast<const void*>(static_cast<uintptr_t>(b)), cntB * 0x34);
    if (a) E->freeFn(E->flag ? a - 4 : a, E->flag ? 0x8f6 : 0x8f8);
    if (b) E->freeFn(E->flag ? b - 4 : b, E->flag ? 0x8f6 : 0x8f8);
    return { total, pool, pool, second };
}
