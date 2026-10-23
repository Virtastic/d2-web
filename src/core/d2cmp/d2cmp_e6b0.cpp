// D2CMP blend-table row blitter (DC6 rows): dst[i] = tbl[dst[i]*256 + src[i]].
// ponytail: register args (EAX=dst, ECX=src, ESI=tbl) taken as explicit params.
#include <cstdint>

// 1.13c D2CMP+0x0E6B0
void d2cmp_e6b0(uint8_t* dst, const uint8_t* src, const uint8_t* tbl, int rows, int stride)
{
    for (;;) {
        uint8_t* next = dst + stride;
        for (;;) {
            uint8_t b = *src++;
            if (b & 0x80) {
                if ((b & 0x7f) == 0) break;  // end of row
                dst += b & 0x7f;             // transparent skip
                continue;
            }
            for (int i = 0; i < b; i++) dst[i] = tbl[dst[i] * 256 + src[i]];
            dst += b; src += b;
        }
        dst = next;
        if (--rows == 0) return;
    }
}
