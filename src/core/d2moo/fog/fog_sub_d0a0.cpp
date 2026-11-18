// 1.13c Fog+0x0D0A0 -- refcounted hash-bucket release: find node by key, drop one ref, unlink+free at <=0.
// Retail layout, 32-bit fields: table +0x10 bucket count, +0x40 bucket array; node {key, refs, next}.
#include <cstdint>

extern "C" void FUN_6ff6b0c0(void** p);  // Fog pool free (takes &ptr)

static inline uint32_t& W32(uint32_t a, uint32_t o) { return *reinterpret_cast<uint32_t*>((uintptr_t)a + o); }

// Returns 1 if key found (ref dropped), 0 if absent.
int fog_hashRelease(uint8_t* t, const uint32_t* key) {
    uint32_t b = *key % *reinterpret_cast<uint32_t*>(t + 0x10);
    uint32_t slot = *reinterpret_cast<uint32_t*>(t + 0x40) + b * 4;
    uint32_t prev = 0, n = W32(slot, 0);
    for (; n; prev = n, n = W32(n, 8))
        if (W32(n, 0) == *key) break;
    if (!n) return 0;
    if ((int32_t)--W32(n, 4) < 1) {
        (prev ? W32(prev, 8) : W32(slot, 0)) = W32(n, 8);
        void* p = (void*)(uintptr_t)n;
        FUN_6ff6b0c0(&p);
    }
    return 1;
}
