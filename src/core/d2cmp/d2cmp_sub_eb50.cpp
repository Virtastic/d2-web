// D2CMP DC6 row-run blit, clipped, through palette remap + 64K blend table. Registers:
// EAX=dst, ESI=blend table, EDI=remap palette (decompile's in_EAX/unaff_ESI/unaff_EDI).
#include <cstdint>

// 1.13c D2CMP+0x0EB50
void d2cmp_eb50(const uint8_t* src, uint8_t* dst, const uint8_t* tbl, const uint8_t* pal,
                int rows, int stride, int x0, int clipL, int clipR)
{
    for (;;) {
        uint8_t* next = dst + stride;
        int x = x0;
        for (;;) {
            uint8_t b = *src++;
            if (b & 0x80) {
                b &= 0x7f;
                if (!b) break;
                x += (int8_t)b; dst += (int8_t)b;
                continue;
            }
            int lo = clipL - x; if (lo < 0) lo = 0;
            int hi = clipR - x; if (b < hi) hi = b;
            for (int i = lo; i < hi; i++) dst[i] = tbl[dst[i] * 256 + pal[src[i]]];
            x += b; src += b; dst += b;
        }
        dst = next;
        if (--rows == 0) return;
    }
}
