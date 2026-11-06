// src/core/d2cmp/d2cmp_dc80.cpp -- 1.13c D2CMP.dll+0xDC80, transcribed from the disassembly (the decompile drops the register arguments of the callees).
// Retail convention: stdcall, five stack words (spr, flag, a2, a3, a4), ret 0x14. Sprite-cache entry loader: allocates a 0xB8-byte entry from the cache
// (0xCC40), fills it from the sprite header, resolves the slot with 0xD7F0 when the flag's bit 0 is clear, then takes a node from 0x16F70.
#include <cstdint>
#include <cstring>

struct D2CMPDC80Env {
    uint32_t enabled;                                                    // DAT 0x6FE77270
    uint32_t cacheWord;                                                  // DAT 0x6FE77240 (ECX at 0x16F70)
    uint32_t** (*alloc)(uint32_t size, uint32_t zero, uint32_t one);     // 0xCC40: EAX = &cache (0x6FE77240), stack (0xB8, 0, 1)
    int32_t (*d7f0)(const uint8_t* spr, uint32_t a3, const uint8_t* flag, uint32_t a2, uint32_t a4);   // 0xD7F0: EAX spr, EBX a3, stack (flag, a2, a4)
    void (*d250)(uint32_t slot, uint32_t one);                           // 0xD250: EAX = &cache, stack (slot, 1)
    void (*cb60)(uint32_t** h);                                          // 0xCB60: EAX = 1, ECX = h, ESI = &cache
    uint32_t* (*node)(const uint8_t* spr, uint32_t* e, const uint8_t* flag);   // 0x16F70: ECX cacheWord, EDI spr, EBX = &table (0x6FE772B0), stack (e, flag); NULL is fatal
};

// 1.13c D2CMP+0x0DC80
extern "C" uint32_t d2cmp_dc80(const uint8_t* spr, uint8_t* flag, uint32_t a2, uint32_t a3, uint32_t a4, const D2CMPDC80Env* env) {
    auto s = [&](int o) { uint32_t v; memcpy(&v, spr + o, 4); return v; };
    if (!env->enabled) return 0;
    uint32_t** h = env->alloc(0xb8, 0, 1);
    if (!h) return 0;
    uint32_t* e = *h;
    for (int i = 0; i < 0x2e; i++) e[i] = 0;
    e[3] = s(0x18); e[6] = s(0x24); e[4] = s(0x1c); e[5] = s(0x20); e[7] = s(0x28);
    e[0x2d] = (uint32_t)(uintptr_t)h;
    if (!(*flag & 1)) {
        e[2] = s(0xc); e[1] = s(0x10);
        const int32_t r = env->d7f0(spr, a3, flag, a2, a4);
        if (r == -1) { env->cb60(h); return 0; }
        env->d250((uint32_t)r, 1);
    }
    uint32_t* n = env->node(spr, *h, flag);
    ((uint32_t*)h)[2] = (uint32_t)(uintptr_t)n;
    n[9] = (uint32_t)(uintptr_t)h;
    uint8_t* b = (uint8_t*)(e + 8);
    if (*flag & 1) { *b |= 1; return 0; }
    *b |= 2;
    return 1;
}
