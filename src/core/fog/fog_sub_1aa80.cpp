// 1.13c Fog+0x1AA80: large-block release. Unlinks the header at (user-4) from
// its hash chain (pool+0x7ac, 0x3ff buckets, key = header % 0x3ff, link = first word) under the pool lock
// (pool+4), then frees the header. Absent from the chain: still freed. Lock/free are seams.
#include <cstdint>
#include <cstdlib>

struct FogBigFreeEnv {
    void (*lock)(uint8_t* cs);    // Fog+0x11690 note + EnterCriticalSection
    void (*unlock)(uint8_t* cs);  // Fog+0x11690 note + LeaveCriticalSection
    void (*free_fn)(void* p);     // _free (Fog+0x1FAD)
};

extern "C" void fog_sub_1aa80(uint8_t* pool, void* user, const FogBigFreeEnv* e) {
    // Retail words are 32-bit: the bucket heads and the chain links are dword addresses.
    const uint32_t hdr = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(user)) - 4;  // retail: (int)hdr % 0x3ff
    uint32_t* buckets = reinterpret_cast<uint32_t*>(pool + 0x7ac);   // retail idiv: the bucket index is the signed remainder
    const int32_t b = static_cast<int32_t>(hdr) % 0x3ff;
    e->lock(pool + 4);
    uint32_t* prev = nullptr;
    for (uint32_t n = buckets[b]; n; ) {
        uint32_t* link = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(n));
        if (n == hdr) {
            if (prev) *prev = *link; else buckets[b] = *link;
            break;
        }
        prev = link;
        n = *link;
    }
    e->unlock(pool + 4);
    e->free_fn(reinterpret_cast<void*>(static_cast<uintptr_t>(hdr)));
}
