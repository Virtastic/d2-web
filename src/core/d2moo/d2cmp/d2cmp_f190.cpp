#include <cstdint>
// 1.13c D2CMP+0xF190
// Palette-mapped RLE row blit: src (ECX), dst (EAX), rows, stride, map (ESI).
// b<0x80: copy b pixels through map; b>0x80: skip b&0x7f; 0x80: end of row.
void d2cmp_f190(const uint8_t* src, uint8_t* dst, int rows, int stride, const uint8_t* map) {
    uint8_t* next = dst + stride;
    for (;;) {
        uint8_t b = *src++;
        if (b < 0x80) { while (b--) *dst++ = map[*src++]; }
        else if (b & 0x7f) dst += b & 0x7f;
        else { if (--rows == 0) return; dst = next; next += stride; }
    }
}
