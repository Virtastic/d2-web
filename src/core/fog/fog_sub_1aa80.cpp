// 1.13c Fog+0x1AA80: large-block release (EAX = user pointer, stack word 0 = pool, ret 4). The header (user-4) is unlinked from its
// hash chain (pool+0x7AC, 0x3FF buckets, bucket = the SIGNED remainder of the header address % 0x3FF, link = the node's first dword)
// under the pool lock (pool+4), then freed whether it was found or not. The pool and the nodes are retail's 32-bit layout, read through
// their 32-bit guest address (a link is a dword, never a host pointer). Lock/free are seams.
#include <cstdint>
#include <cstring>

struct FogBigFreeEnv {
    void (*lock)(uint8_t* cs);    // Fog+0x11690 note + EnterCriticalSection
    void (*unlock)(uint8_t* cs);  // Fog+0x11690 note + LeaveCriticalSection
    void (*free_fn)(void* p);     // _free (Fog+0x1FAD)
};

namespace {
inline uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline void wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
}

extern "C" void fog_sub_1aa80(uint8_t* pool, void* user, const FogBigFreeEnv* e) {
    const uint32_t hdr = (uint32_t)(uintptr_t)user - 4;
    const uint32_t slot = (uint32_t)(uintptr_t)pool + 0x7ac + (uint32_t)(((int32_t)hdr % 0x3ff) * 4);   // idiv: signed remainder
    e->lock(pool + 4);
    uint32_t prev = 0;
    for (uint32_t n = rd32(slot); n; prev = n, n = rd32(n)) {
        if (n == hdr) {
            if (prev) wr32(prev, rd32(n)); else wr32(slot, rd32(n));
            break;
        }
    }
    e->unlock(pool + 4);
    e->free_fn((void*)(uintptr_t)hdr);
}
