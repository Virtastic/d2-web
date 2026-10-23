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
    void** hdr = reinterpret_cast<void**>(static_cast<uint8_t*>(user) - 4);  // retail: (int)hdr % 0x3ff
    void** buckets = reinterpret_cast<void**>(pool + 0x7ac);
    e->lock(pool + 4);
    void** prev = nullptr;
    for (void** n = static_cast<void**>(buckets[reinterpret_cast<uintptr_t>(hdr) % 0x3ff]); n; prev = n, n = static_cast<void**>(*n)) {
        if (n == hdr) {
            if (prev) *prev = *n; else buckets[reinterpret_cast<uintptr_t>(hdr) % 0x3ff] = static_cast<void**>(*n);
            break;
        }
    }
    e->unlock(pool + 4);
    e->free_fn(hdr);
}
