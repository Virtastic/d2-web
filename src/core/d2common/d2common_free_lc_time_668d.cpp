// 1.13c D2Common+0x0668D ___free_lc_time (MSVC 2003 CRT): frees the 43 u32 pointer slots (0x00-0x2A)
// of the time-locale block in retail's call order (1-6, 0, 8-0xD, 7, 0xE-0x2A); NULL block: no-op.
// Callee: _free, D2Common+0x4C0D = d2common_free (d2common_crt_tiny.cpp), which ignores NULL.
#include <cstdint>

struct D2CommonFreeHost;
extern "C" void d2common_free(D2CommonFreeHost* h, void* p);

extern "C" void d2common_free_lc_time_668d(const uint32_t* block, D2CommonFreeHost* h) {
    if (!block) return;
    static const uint8_t kOrder[43] = {
        1, 2, 3, 4, 5, 6, 0, 8, 9, 0xA, 0xB, 0xC, 0xD, 7,
        0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B,
        0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A,
    };
    for (uint32_t i = 0; i < 43; ++i) d2common_free(h, (void*)(uintptr_t)block[kOrder[i]]);
}
