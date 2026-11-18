// 1.13c D2CMP+0xB930 (export #10042, 195 bytes): build a palette-shift block (0x6C327 bytes) from a
// 256-entry 4-byte palette.
#include <cstring>
#include "d2cmp_palstate.h"
// 0x6FE340D8: the 13 filter triples 0x9ED0 reads (copied to block+0x6B600; the first is not used there).
static const uint8_t kFilterTriples[39] = {
    0xff, 0xff, 0xff, 0xff, 0x4d, 0x4d, 0x00, 0xff, 0x00, 0x69, 0x69, 0xff, 0xc7, 0xb3, 0x77, 0x69, 0x69, 0x69, 0x00, 0x00,
    0x00, 0xd0, 0xc2, 0x7d, 0xff, 0xa8, 0x00, 0xff, 0xff, 0x64, 0x00, 0x80, 0x00, 0xae, 0x00, 0xff, 0x00, 0xc8, 0x00};
// 1.13c D2CMP+0x0B930
// Fatal 0x2AB on a null palette. The block is allocated (VirtualAlloc, not zeroed by us beyond what
// the seam returns), the palette copied to its first 0x400 bytes, and the tables built in retail's
// order: HSL (0xA830, a 256x3-double local), 0xA070, 0x9E60, 0x9DC0, 0xA640, 0x9FA0, 0xAB60, 0xAAD0
// (boosts the local HSL in place), 0xA490, the filter triples, 0x9ED0. Returns the block.
extern "C" uint8_t* d2cmp_b930(const D2CmpPalEnv* env, const uint8_t* pal) {
    if (pal == nullptr) env->fatal(0x2ab);
    uint8_t* block = env->valloc(0x6c327);
    std::memcpy(block, pal, 0x400);
    double hsl[0x100 * 3];
    d2cmp_a830(env, hsl, block);
    d2cmp_a070(block);
    d2cmp_9e60(block);
    d2cmp_9dc0(block);
    d2cmp_a640(block);
    d2cmp_9fa0(block);
    d2cmp_ab60(block, hsl);
    d2cmp_aad0(hsl, block);
    d2cmp_a490(block);
    std::memcpy(block + 0x6b600, kFilterTriples, sizeof kFilterTriples);
    d2cmp_9ed0(block);
    return block;
}
