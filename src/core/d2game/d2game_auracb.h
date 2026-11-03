// 1.13c D2Game+0x3CC30 / +0x3CD00: the two aura callbacks that spawn a flag-0x20 hit sub-missile on each aura target.
// Written from the 1.13c decompile as a rule over an environment (every callee is a seam in retail argument order).
// Register convention: ecx = D2AuraCallbackStrc (+0 game, +8 counter, +0xC args), edx = target unit. Returns 0 (stop) or 1.
#pragma once
#include <cstdint>
#include <cstring>

struct D2GameAuraCbEnv {
    int32_t (*posX)(void* rec);                          // D2Common #10867 (0xA454)
    int32_t (*posY)(void* rec);                          // D2Common #10750 (0xA3B8)
    void (*createMissile)(uint32_t game, uint32_t* req); // 0x6F930 with the 23-dword request
};

namespace auracb {
static inline uint32_t rd(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void* ptr(const void* p, uint32_t o) { return (void*)(uintptr_t)rd(p, o); }

// positiveCapOnly: 3CC30 stops only for a positive cap (nParam > 0 && nParam <= counter); 3CD00 stops whenever nParam <= counter.
static inline int32_t Run(const void* aura, const void* tgt, bool positiveCapOnly, const D2GameAuraCbEnv* e)
{
    const void* a = ptr(aura, 0xC);
    const int32_t cap = (int32_t)rd(a, 0x10), counter = (int32_t)rd(aura, 8);
    if (positiveCapOnly ? (cap > 0 && cap <= counter) : (cap <= counter)) return 0;
    uint32_t req[23] = {};
    req[0] = 0x20;
    req[1] = rd(a, 0);
    req[2] = rd(a, 4);
    req[0xB] = rd(a, 8);
    req[0xC] = rd(a, 0xC);
    req[4] = rd(a, 0x14);
    const uint32_t type = rd(tgt, 0);
    void* rec = ptr(tgt, 0x2C);
    const bool pet = type == 2 || (type > 3 && type < 6);
    req[7] = pet ? rd(rec, 0xC) : rec ? (uint32_t)e->posX(rec) : 0;
    req[8] = pet ? rd(rec, 0x10) : rec ? (uint32_t)e->posY(rec) : 0;
    e->createMissile(rd(aura, 0), req);
    return 1;
}
}
