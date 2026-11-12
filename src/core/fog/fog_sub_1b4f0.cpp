// 1.13c Fog+0x1B4F0: pick the pool block to allocate from. Custom-register call (EBX=allocator, ESI=pool,
// EAX=start bit for 1ABB0, arg=out block**). Callees are seams.
// Every pool and block field is a 32-bit retail word (block links and the out slot are dword addresses), read here as uint32_t.
#include <cstdint>

// pool+0x1c = units per block, +0x28 = first block, +0x2c = current block (offsets as in decompile)
// block: +0 data, +4 bits, +8 used, +0xc next, +0x10 prev

struct fog_1b4f0_api {
    uint32_t* (*newBlock)(void* alloc, void* pool);        // Fog+0x1B390
    int (*findFree)(uint32_t start, uint32_t* bits);       // Fog+0x1ABB0 (<0 = none)
};

static inline uint32_t* w32(uint32_t addr) { return reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(addr)); }
static inline uint32_t a32(const void* p) { return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p)); }

int fog_sub_1b4f0(const fog_1b4f0_api* a, void* alloc, uint8_t* pool, uint32_t start, uint32_t** out) {
    uint32_t* first = reinterpret_cast<uint32_t*>(pool + 0x28);
    uint32_t* cur = reinterpret_cast<uint32_t*>(pool + 0x2c);
    uint32_t* outw = reinterpret_cast<uint32_t*>(out);
    const uint32_t b = *first;
    if (!b) {
        uint32_t* n = a->newBlock(alloc, pool);
        if (n) { *cur = a32(n); *first = a32(n); }
        *outw = a32(n);
        return 0;
    }
    if (static_cast<int32_t>(w32(b)[2]) < *reinterpret_cast<int32_t*>(pool + 0x1c)) {
        int r = a->findFree(start, w32(w32(b)[1]));
        if (r >= 0) { *outw = b; return r; }
    }
    uint32_t* n = a->newBlock(alloc, pool);
    if (n) {
        n[4] = *first;
        w32(*first)[3] = a32(n);
        *first = a32(n);
    }
    *outw = a32(n);
    return 0;
}
