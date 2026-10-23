// 1.13c D2CMP+0x0e480: clipped, table-blended draw of an RLE row stream.
#include <cstdint>

// rows: row count; stride: dst advance per row; x0: starting x; clipL/clipR: visible x range [clipL, clipR).
// tbl: 64K blend table indexed [dst<<8 | src]. src stream: byte>=0x80 skip (b&0x7f, 0 ends row); 1..0x7f literal run.
extern "C" void d2cmp_sub_e480(const int8_t* src, uint8_t* dst, const uint8_t* tbl,
                               int rows, int stride, int x0, int clipL, int clipR) {
    do {
        uint8_t* rowStart = dst;
        int x = x0;
        for (;;) {
            uint8_t b = *src++;
            if (b & 0x80) {
                b &= 0x7f;
                if (!b) break;
                x += (int8_t)b; dst += (int8_t)b;
                continue;
            }
            int n = (int8_t)b;
            int s = clipL - x; if (s < 0) s = 0;
            int e = clipR - x; if (n < e) e = n;
            for (int i = s; i < e; i++) dst[i] = tbl[src[i] + dst[i] * 256];
            x += n; src += n; dst += n;
        }
        dst = rowStart + stride;
    } while (--rows);
}
