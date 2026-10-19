#include <cstdint>

// 1.13c D2Gfx+0x8E30
// Retail: EAX=stream, ESI=palette, ECX=rows, stack=destination;
// pitch is global +0x1477C. Native callers pass it explicitly.
extern "C" void d2gfx_sub_8e30(const uint8_t* src, const uint8_t* palette,
                              uint32_t rows, uint8_t* dst, int32_t pitch) {
    uint8_t* row = dst;
    while (rows != 0) {
        const uint32_t skip = src[0], count = src[1];
        src += 2;
        if (skip == 0 && count == 0) {
            --rows;
            row += pitch;
            dst = row;
            continue;
        }
        dst += skip;
        // Retail's fall-through switch writes back-to-front and skips counts >32.
        if (count <= 32) {
            for (uint32_t i = count; i != 0; --i)
                dst[i - 1] = palette[src[i - 1]];
        }
        dst += count;
        src += count;
    }
}
