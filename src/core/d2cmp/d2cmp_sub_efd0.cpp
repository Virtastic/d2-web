// D2CMP DC6-style RLE row blit through a 256-byte remap table, clipped to x in [clipL, clipR).
#include <cstdint>

// 1.13c D2CMP+0x0EFD0
// src=ECX, dst=EAX, tbl=ESI (register args); x0 = starting column.
void d2cmp_efd0(const uint8_t* src, uint8_t* dst, const uint8_t* tbl,
                int rows, int pitch, int x0, int clipL, int clipR)
{
    do {
        uint8_t* rowEnd = dst + pitch;   // pitch added to the row's origin dst
        int x = x0;
        for (;;) {
            uint8_t b = *src++;
            if (b & 0x80) {
                b &= 0x7f;
                if (!b) break;           // end of row
                x += b; dst += b;        // transparent skip
                continue;
            }
            int lo = clipL - x; if (lo < 0) lo = 0;
            int hi = clipR - x; if (hi > b) hi = b;
            for (int i = lo; i < hi; i++) dst[i] = tbl[src[i]];
            x += b; src += b; dst += b;
        }
        dst = rowEnd;
    } while (--rows);
}
