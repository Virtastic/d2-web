// 1.13c Storm+0x322F0: TGA pixel unpack (raw type 1/2, RLE type 9/10) with flip flags.
#include <cstdint>
#include <cstring>

// hdr: TGA header (+2 type, +0xC w, +0xE h, +0x10 bpp, +0x11 descriptor). limit = EAX input (dest byte cap).
extern "C"
uint32_t Storm_Sub322F0(const uint8_t* src, const uint8_t* hdr, int32_t srcLen, uint8_t* dst, uint32_t limit) {
    uint32_t w = hdr[0xC] | (hdr[0xD] << 8), h = hdr[0xE] | (hdr[0xF] << 8);
    int32_t bpp = (hdr[0x10] + 7) >> 3;
    uint32_t total = (uint32_t)bpp * h * w;
    int32_t cap = (int32_t)limit <= (int32_t)total ? (int32_t)limit : (int32_t)total;
    uint8_t d = hdr[0x11] >> 4;
    bool flipX = d & 1, top = (d >> 1) & 1;
    int32_t step = flipX ? -bpp : bpp;
    int32_t rowAdj = top ? (bpp - step) * (int32_t)w : -((step + bpp) * (int32_t)w);
    if (flipX) dst += (w - 1) * bpp;
    if (!top) dst += (h - 1) * bpp * w;
    uint32_t left = w;
    auto put = [&](const uint8_t* p) {
        if (bpp >= 1 && bpp <= 4) memcpy(dst, p, bpp);
        dst += step;
        if (--left == 0) { dst += rowAdj; left = w; }
    };
    uint8_t t = hdr[2];
    if (t == 1 || t == 2) {
        if (cap > srcLen) return 0;
        if (!flipX && top) { memcpy(dst, src, cap); return 1; }
        for (int32_t n = cap / bpp; n > 0; n--, src += bpp) put(src);
        return 1;
    }
    if (t != 9 && t != 10) return 0;
    int32_t n = cap / bpp;
    const uint8_t* end = src + srcLen;
    while (n != 0 && src < end) {
        uint8_t c = *src++;
        int32_t run = (c & 0x7f) + 1;
        if (run > n) run = n;
        n -= run;
        if (c & 0x80) {
            if (end < src + bpp) return 0;
            for (; run; run--) put(src);
            src += bpp;
        } else {
            if (end < src + (int64_t)run * bpp) return 0;
            for (; run; run--, src += bpp) put(src);
        }
    }
    return 1;
}
