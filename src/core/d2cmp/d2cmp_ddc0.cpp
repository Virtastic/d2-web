// src/core/d2cmp/d2cmp_ddc0.cpp -- 1.13c D2CMP.dll+0xDDC0, transcribed from the disassembly.
// Retail convention: stdcall (spr, flag*, a3, a4, a5), ret 0x14. Builds one sprite-cache entry: CC40 allocates the 0xB8-byte handle (EAX = &cache 0x67240, stack 0xB8, 0, 0);
// the entry (*handle) is zeroed and filled from spr; with flag bit 0 clear D9F0 (EDI = entry, stack spr, a3, a4, a5) loads it; 16F70 (ECX = cache word, EBX = &table 0x672B0,
// EDI = spr, stack entry, flag) returns the cache node (fatal 0x5F3 on null); with flag bit 0 set the entry is marked and 0 returned; else CFC0 (EAX = entry, stack flag, spr)
// runs when entry[10] is still 0 and D310 (EAX = entry, EDI = spr) yields spr[0xf] and the result.
#include <cstdint>
#include <cstring>
#include <unistd.h>

struct D2CMPDDC0Env {
    uint32_t cacheWord;                                                                     // [0x67240]
    uint32_t** (*alloc)(uint32_t size, uint32_t b, uint32_t c);                             // 0xCC40
    void (*d9f0)(uint8_t* spr, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t* entry);     // 0xD9F0
    uint32_t* (*node)(uint32_t* entry, uint8_t* flag, uint8_t* spr);                        // 0x16F70
    void (*cfc0)(uint32_t* entry, uint8_t* flag, uint8_t* spr);                             // 0xCFC0
    uint32_t (*d310)(uint32_t* entry, uint8_t* spr);                                        // 0xD310
};

static inline uint32_t dd_rd(const uint8_t* b, uint32_t o) { uint32_t v; std::memcpy(&v, b + o, 4); return v; }

// 1.13c D2CMP+0x0DDC0
extern "C" uint32_t d2cmp_ddc0(uint8_t* spr, uint8_t* flag, uint32_t a3, uint32_t a4, uint32_t a5, const D2CMPDDC0Env* env) {
    if (!flag) return 0;
    uint32_t** h = env->alloc(0xb8, 0, 0);
    if (!h) return 0;
    uint32_t* e = *h;
    std::memset(e, 0, 0x2e * 4);
    e[0] = dd_rd(spr, 8); e[3] = dd_rd(spr, 0x18); e[6] = dd_rd(spr, 0x24); e[4] = dd_rd(spr, 0x1c); e[5] = dd_rd(spr, 0x20); e[7] = dd_rd(spr, 0x28);
    e[0x2d] = (uint32_t)(uintptr_t)h;
    if (!(*flag & 1)) { e[2] = dd_rd(spr, 0xc); e[1] = dd_rd(spr, 0x10); env->d9f0(spr, a3, a4, a5, e); }
    uint32_t* n = env->node(e, flag, spr);
    if (!n) _exit(-1);                                                                      // fatal 0x5F3
    ((uint32_t*)h)[2] = (uint32_t)(uintptr_t)n;
    n[9] = (uint32_t)(uintptr_t)h;
    if (*flag & 1) { *(uint8_t*)(e + 8) |= 1; return 0; }
    if (e[10] == 0) env->cfc0(e, flag, spr);
    const uint32_t r = env->d310(e, spr);
    std::memcpy(spr + 0x3c, &r, 4);
    return r != 0;
}
