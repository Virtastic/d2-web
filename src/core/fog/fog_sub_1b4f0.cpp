// 1.13c Fog+0x1B4F0: pick the pool block to allocate from. Custom-register call (EBX=allocator, ESI=pool, one stack arg =
// out block*, ret 4). The pool and its blocks are retail's 32-bit layout, read through their 32-bit guest address:
// pool+0x1c = units per block, +0x28 = first block, +0x2c = current block; block +4 = bit map, +8 = used, +0xc = prev, +0x10 = next.
// When the first block has room, 1ABB0 is called with EAX = the pool's units per block (the incoming EAX is overwritten by
// `mov eax,[esi+0x1c]` before the compare) and ECX = the block's bit map. Callees are seams.
#include <cstdint>
#include <cstring>

struct fog_1b4f0_api {
    uint32_t* (*newBlock)(void* alloc, void* pool);        // Fog+0x1B390
    int (*findFree)(uint32_t units, uint32_t* bits);       // Fog+0x1ABB0 (<0 = none)
};

namespace {
inline uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline void wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
}

int fog_sub_1b4f0(const fog_1b4f0_api* a, void* alloc, uint8_t* pool, uint32_t /*eax: overwritten*/, uint32_t** out) {
    const uint32_t p = (uint32_t)(uintptr_t)pool, o = (uint32_t)(uintptr_t)out;
    const uint32_t b = rd32(p + 0x28);
    if (!b) {
        const uint32_t n = (uint32_t)(uintptr_t)a->newBlock(alloc, pool);
        if (n) { wr32(p + 0x2c, n); wr32(p + 0x28, n); }
        wr32(o, n);
        return 0;
    }
    const uint32_t units = rd32(p + 0x1c);
    if ((int32_t)rd32(b + 8) < (int32_t)units) {
        const int r = a->findFree(units, (uint32_t*)(uintptr_t)rd32(b + 4));
        if (r >= 0) { wr32(o, b); return r; }
    }
    const uint32_t n = (uint32_t)(uintptr_t)a->newBlock(alloc, pool);
    if (n) {
        wr32(n + 0x10, rd32(p + 0x28));
        wr32(rd32(p + 0x28) + 0xc, n);
        wr32(p + 0x28, n);
    }
    wr32(o, n);
    return 0;
}
