#include <cstdint>

// Prior art: D2MOO CmnSubtile.cpp's diamond row tables and palette mapping.
// The three tables below were checked against 1.13c RVAs 0x10C44/0x10C80/0x10CBC.
// 1.13c D2Gfx+0x93E0
// Retail stack=destination, EBX=packed pixels, ESI=palette; +0x1477C=pitch.
// Native pointers replace register-held x86 addresses, pitch remains retail int32.
extern "C" void d2gfx_sub_93e0(uint8_t* dst, const uint8_t* src,
                              const uint8_t* palette, int32_t pitch) {
    static constexpr int32_t indent[15] =
        {14,12,10,8,6,4,2,0,2,4,6,8,10,12,14};
    static constexpr int32_t width[15] =
        {4,8,12,16,20,24,28,32,28,24,20,16,12,8,4};
    static constexpr int32_t offset[15] =
        {0,4,12,24,40,60,84,112,144,172,196,216,232,244,252};
    for (int32_t row = 0; row < 15; ++row) {
        uint8_t* out = dst + indent[row];
        const uint8_t* in = src + offset[row];
        // Preserve retail's descending stores, including when buffers overlap.
        for (int32_t pixel = width[row]; pixel != 0; --pixel)
            out[pixel - 1] = palette[in[pixel - 1]];
        dst += pitch;
    }
}
