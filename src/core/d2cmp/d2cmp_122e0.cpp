// 1.13c D2CMP+0x122E0 (#10021): cel pointer for a cel context's frame and direction
// Context @+0x00 frame, @+0x34 cell file, @+0x40 direction;
// the cell file is a DC6-shaped header: [0] type 6, [4] directions, [5] frames, [6..] cel pointers.
// The probe src/core/fn/d2cmp_122e0.cpp drives this body.
#include <cstdint>
#include <unistd.h>

namespace {

// Direction-order permutation read by the routine from .rdata RVA 0x1E4D8 (row 7 of an 8x64 table).
constexpr uint8_t kDir64[64] = {
    4, 32, 16, 33, 8, 34, 17, 35, 0, 36, 18, 37, 9, 38, 19, 39,
    5, 40, 20, 41, 10, 42, 21, 43, 1, 44, 22, 45, 11, 46, 23, 47,
    6, 48, 24, 49, 12, 50, 25, 51, 2, 52, 26, 53, 13, 54, 27, 55,
    7, 56, 28, 57, 14, 58, 29, 59, 3, 60, 30, 61, 15, 62, 31, 63,
};

// Table row = lowest set bit of `directions` + 1. Rows 4..7 are strided subsamples of kDir64 with
// the index taken modulo 64 (the last group wraps); row 3 is the four-way split; the rest are zero.
uint32_t slotFor(uint32_t directions, uint32_t dir) {
    uint32_t row = directions ? (uint32_t)__builtin_ctz(directions) + 1 : 0;
    switch (row) {
    case 3: return ((dir + 8) >> 4) & 3;
    case 4: return kDir64[8 * (((dir + 4) >> 3) & 7)];
    case 5: return kDir64[4 * (((dir + 2) >> 2) & 15)];
    case 6: return kDir64[2 * (((dir + 1) >> 1) & 31)];
    case 7: return kDir64[dir & 63];
    default: return 0;
    }
}

}  // namespace

// Fatal paths (message via Ordinal 10265/10024, then `push -1; call _exit`) are not reproduced
// beyond the exit, as in sub_11AC0.
// 1.13c D2CMP+0x122E0
extern "C" uint32_t d2cmp_122e0(uint32_t* ctx) {
    if (!ctx) _exit(-1);
    uint32_t* f = (uint32_t*)(uintptr_t)ctx[0xD];      // +0x34
    uint32_t dir = ctx[0x10];                          // +0x40
    uint32_t frame = ctx[0];                           // +0x00
    if (!f || f[0] != 6 || dir >= 0x40 || frame > f[5]) _exit(-1);
    return f[6 + slotFor(f[4], dir) * f[5] + frame];
}
