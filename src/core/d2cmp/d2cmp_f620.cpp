// D2CMP+0xF620 is byte-identical in body to +0xEB50 (the decompiles differ only in names): same DC6 row-run blend blit.
#include <cstdint>
void d2cmp_eb50(const uint8_t*, uint8_t*, const uint8_t*, const uint8_t*, int, int, int, int, int);

// 1.13c D2CMP+0xF620
void d2cmp_f620(const uint8_t* src, uint8_t* dst, const uint8_t* tbl, const uint8_t* pal,
                int rows, int stride, int x0, int clipL, int clipR)
{
    d2cmp_eb50(src, dst, tbl, pal, rows, stride, x0, clipL, clipR);
}
