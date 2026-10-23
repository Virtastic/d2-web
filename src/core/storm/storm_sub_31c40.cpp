// 1.13c Storm+0x31C40: RLE (0xC0 run / literal) unpack into a strided, interleaved plane buffer.
#include <cstdint>

extern "C"
void Storm_Sub31C40(uint32_t dstLen, uint32_t srcLen, uint8_t* dst, uint8_t* src, uint32_t width, uint32_t stride,
                    int32_t ch) {
    uint8_t* const dstEnd = dst + dstLen;
    uint8_t* const srcEnd = src + srcLen;
    int32_t rowStep = (int32_t)(width * (uint32_t)ch);
    uint32_t x = 0;
    int32_t plane = ch - 1;
    for (;;) {
        if (dstEnd <= dst + x || srcEnd <= src) return;  // decompile tests the stale cursor pbVar11 == dst+x
        uint8_t b = *src++;
        if (b < 0xC0) {
            if (x < width) {
                if (dst + (int64_t)plane + (int64_t)x * ch < dstEnd) dst[(int64_t)x * ch + plane] = b;
            }
            if (++x >= stride) {
                x -= stride;
                if (--plane < 0) { plane = ch - 1; dst += rowStep; }
            }
        } else {
            uint32_t cnt = b & 0x3F;
            uint32_t rem = (uint32_t)(dstEnd - dst);
            if (rem <= cnt) cnt = rem;
            uint8_t v = *src++;
            int32_t left = (int32_t)(dstEnd - dst);
            do {
                if (width <= x) {
                    uint32_t n = stride - x;
                    if (cnt < n) n = cnt;
                    x += n;
                    if (stride <= x) {
                        x -= stride;
                        if (--plane < 0) { dst += rowStep; plane = ch - 1; left -= rowStep; }
                    }
                    cnt -= n;
                    if (cnt == 0) break;
                }
                uint32_t n = width - x;
                if (cnt < n) n = cnt;
                int32_t i = (int32_t)(x * (uint32_t)ch) + plane;
                int32_t lim = (int32_t)(n * (uint32_t)ch);
                if (left < lim) lim = left;
                lim += i;
                for (; i < lim; i += ch) dst[i] = v;
                x += n;
                if (stride <= x) {
                    x -= stride;
                    if (--plane < 0) { dst += rowStep; left -= rowStep; plane = ch - 1; }
                }
                cnt -= n;
            } while (cnt != 0);
        }
    }
}
