// 1.13c Fog+0x08B9E ___free_lc_time(p): frees the 43 slots of a time-string table through _free
// (Fog+0x1FAD) in retail's order. Each slot goes to _free, null slots included (retail does not test
// them); p itself is tested once. The port takes the free seam so a probe can log the calls.
#include <cstdint>

extern "C" void fog_free_lc_time_8b9e(const uint32_t* p, void (*freeFn)(void*)) {
    static const uint8_t kOrder[43] = {
        1, 2, 3, 4, 5, 6, 0, 8, 9, 10, 11, 12, 13, 7,
        14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    };
    if (!p) return;
    for (uint8_t slot : kOrder) freeFn((void*)(uintptr_t)p[slot]);
}
