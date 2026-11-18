// 1.13c Storm+0x325C0 -- BMP palette -> 256 BGRA-ordered dwords (thiscall; hdr: +3 first, +5 count, +7 bpp).
#include <cstdint>
#include <cstring>
uint32_t Storm_Sub325C0(const uint8_t* hdr, const uint8_t* src, int srcLen, uint8_t* out) {
    uint16_t first, cnt;
    memcpy(&first, hdr + 3, 2);
    memcpy(&cnt, hdr + 5, 2);
    uint32_t step = (hdr[7] + 7) >> 3;
    if ((int)(cnt * step) - srcLen != 0) return 0;
    memset(out, 0, 1024);
    for (uint32_t i = 0; i < cnt; i++, src += step) {
        uint8_t* d = out + (first + i) * 4;
        d[0] = src[2]; d[1] = src[1]; d[2] = src[0];
        d[3] = step < 4 ? 0 : src[3];
    }
    return 1;
}
