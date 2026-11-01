// D2Common 0x120F0 : grow a unit-find record's array by 15 slots. Retail layout:
// +0x00 pool, +0x04 array, +0x1C max entries. The Fog pool realloc (ECX = pool, EDX = old array; stack size, file,
// line 0xC5, 0) is injected so a probe can record it; the record pointer arrives in ESI.
#include <cstdint>
#include <cstring>
#include "d2common_120f0.h"
// 1.13c D2Common+0x120F0
extern "C" void D2Common_120F0_Grow(uint32_t rec, uint32_t (*realloc_)(uint32_t pool, uint32_t old, uint32_t size, uint32_t line)) {
    uint32_t pool, old, n;
    std::memcpy(&pool, (const void*)(uintptr_t)rec, 4);
    std::memcpy(&old, (const void*)(uintptr_t)(rec + 4), 4);
    std::memcpy(&n, (const void*)(uintptr_t)(rec + 0x1c), 4);
    n += 15;
    const uint32_t p = realloc_(pool, old, n * 4, 0xc5);
    std::memcpy((void*)(uintptr_t)(rec + 0x1c), &n, 4);
    std::memcpy((void*)(uintptr_t)(rec + 4), &p, 4);
}
