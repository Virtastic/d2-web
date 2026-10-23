// 1.13c Fog+0x1B4F0: pick the pool block to allocate from. Custom-register call (EBX=allocator, ESI=pool,
// EAX=start bit for 1ABB0, arg=out block**). Callees are seams.
#include <cstdint>

struct fog_pool_blk { uint32_t* data; uint32_t* bits; uint32_t used; fog_pool_blk* next; fog_pool_blk* prev; void* pool; };
struct fog_pool { uint8_t pad[0x1c]; int32_t nUnits; uint8_t pad2[0x08]; fog_pool_blk* head; fog_pool_blk* cur; };
// pool+0x1c = units per block, +0x28 = first block, +0x2c = current block (offsets as in decompile)

struct fog_1b4f0_api {
    uint32_t* (*newBlock)(void* alloc, void* pool);        // Fog+0x1B390
    int (*findFree)(uint32_t start, uint32_t* bits);       // Fog+0x1ABB0 (<0 = none)
};

int fog_sub_1b4f0(const fog_1b4f0_api* a, void* alloc, uint8_t* pool, uint32_t start, uint32_t** out) {
    uint32_t** first = (uint32_t**)(pool + 0x28);
    uint32_t** cur = (uint32_t**)(pool + 0x2c);
    uint32_t* b = *first;
    if (!b) {
        uint32_t* n = a->newBlock(alloc, pool);
        if (n) { *cur = n; *first = n; }
        *out = n;
        return 0;
    }
    if ((int32_t)b[2] < *(int32_t*)(pool + 0x1c)) {
        int r = a->findFree(start, (uint32_t*)(uintptr_t)b[1]);
        if (r >= 0) { *out = b; return r; }
    }
    uint32_t* n = a->newBlock(alloc, pool);
    if (n) {
        n[4] = (uint32_t)(uintptr_t)*first;
        *(uint32_t**)((uint8_t*)*first + 0xc) = n;
        *first = n;
    }
    *out = n;
    return 0;
}
