#include <cstdint>
// UTF-8 sequence table at D2Lang+0x102E8 (cmask, cval, shift, pad, lmask, lval); terminated by cmask 0.
struct D2LangU8Row { uint16_t cmask, cval, shift, pad; int32_t lmask, lval; };
static const D2LangU8Row kU8[] = {
    {128, 0, 0, 0, 127, 0},          {224, 192, 6, 0, 2047, 128},      {240, 224, 12, 0, 65535, 2048},
    {248, 240, 18, 0, 2097151, 65536}, {252, 248, 24, 0, 67108863, 2097152}, {254, 252, 30, 0, 2147483647, 67108864},
    {0, 0, 0, 0, 0, 0}};

// 1.13c D2Lang+0x08C40: decode one UTF-8 char from src (len bytes available); *out = code unit; returns bytes used or -1.
extern "C" int d2lang_8c40(const uint8_t* src, int len, uint16_t* out) {
    if (len < 1) return -1;
    int n = 0; uint32_t v = (uint32_t)(int16_t)src[0]; uint8_t b0 = src[0];
    for (const D2LangU8Row* r = kU8; r->cmask; ++r) {
        ++n;
        if ((uint16_t)(r->cmask & b0) == r->cval) {
            if ((int32_t)(v & (uint32_t)r->lmask) < r->lval) return -1;
            *out = (uint16_t)(v & (uint32_t)r->lmask);
            return n;
        }
        if (len <= n) return -1;
        uint8_t c = *++src ^ 0x80;
        if (c & 0xc0) return -1;
        v = v << 6 | c;
    }
    return -1;
}

// 1.13c D2Lang+0x08DC0: encode code unit ch as UTF-8 into dst; returns bytes written or -1.
extern "C" int d2lang_8dc0(uint8_t* dst, uint16_t ch) {
    int n = 0;
    const D2LangU8Row* r = kU8;
    for (; r->cmask; ++r) { ++n; if ((int)ch <= r->lmask) break; }
    if (!r->cmask) return -1;
    int s = (int16_t)r->shift;
    *dst = (uint8_t)((int)ch >> (s & 31)) | (uint8_t)r->cval;
    while (s > 0) { s -= 6; *++dst = (uint8_t)(((int)ch >> (s & 31)) & 0x3f) | 0x80; }
    return n;
}
