// 1.13c D2CMP+0x9ED0 (194 bytes): 12 colour-filter tables at block+0x6B727 from the triples at +0x6B603.
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x09ED0
// Row r = 0..11 uses the triple t = block+0x6B603+3r. Entry i takes v = block[i*4] (the first byte
// only, for all three channels): (t[0]*v/255, t[1]*v/255, t[2]*v/255), nearest of the 256 entries.
extern "C" void d2cmp_9ed0(uint8_t* block) {
    const uint8_t* t = block + 0x6b603;
    uint8_t* out = block + 0x6b727;
    for (int32_t r = 0; r < 12; ++r, t += 3, out += 0x100)
        for (uint32_t i = 0; i < 0x100; ++i) {
            uint32_t v = block[i * 4];
            out[i] = (uint8_t)d2cmp_sub_9d30(block, 0x100, (int32_t)((t[0] * v / 0xff) & 0xff),
                                             (int32_t)((t[1] * v / 0xff) & 0xff), (int32_t)((t[2] * v / 0xff) & 0xff));
        }
}
