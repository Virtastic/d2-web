// D2CMP RLE row blits through a remap table (e860) and two chained tables (f2c0).
// Args as sub_E480: x0 start x, [clipL, clipR) visible range; src byte>=0x80 skip (0 ends row), else literal run.
#include <cstdint>

static void rows_(const int8_t* src, uint8_t* dst, const uint8_t* t1, const uint8_t* t2,
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
            for (int i = s; i < e; i++) dst[i] = t2 ? t2[t1[(uint8_t)src[i]]] : t1[(uint8_t)src[i]];
            x += n; src += n; dst += n;
        }
        dst = rowStart + stride;
    } while (--rows);
}

// 1.13c D2CMP+0x0e860
extern "C" void d2cmp_sub_e860(const int8_t* src, uint8_t* dst, const uint8_t* tbl,
                               int rows, int stride, int x0, int clipL, int clipR) {
    rows_(src, dst, tbl, nullptr, rows, stride, x0, clipL, clipR);
}

// 1.13c D2CMP+0x0f2c0
extern "C" void d2cmp_sub_f2c0(const int8_t* src, uint8_t* dst, const uint8_t* tbl1, const uint8_t* tbl2,
                               int rows, int stride, int x0, int clipL, int clipR) {
    rows_(src, dst, tbl1, tbl2, rows, stride, x0, clipL, clipR);
}
