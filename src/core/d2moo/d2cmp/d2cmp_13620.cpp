// D2CMP Codec.cpp: re-encode one decoded DCC frame as a DC6-style run-length sprite body.
#include <cstring>
#include "d2cmp_dcc.h"

// 1.13c D2CMP+0x13620
// Retail: EAX = output cursor, stack (frame, sprite header), ret 8. Rows go bottom to top
// (a top-down frame is walked from its last row). Per row: a transparent run (index 0) is
// 0x80|n, n <= 0x7F; an opaque run is n then n bytes; trailing transparency is dropped; each
// row ends with 0x80. The output must end exactly at header + *(u32*)(header+0x1C) + 0x20,
// otherwise Ordinal_10029("Sprite Decompression Error") and 0.
int32_t d2cmp_13620(uint8_t* out, const DccFrame* f, const uint8_t* hdr, void (*log)(const char*)) {
    const uint8_t* row = f->pix;
    int32_t step = f->stride;
    if (f->bottomUp == 0) { row += (f->h - 1) * step; step = -step; }
    for (int32_t y = 0; y < f->h; y++) {
        int32_t x = 0;
        if (0 < f->w) {
            while (x < f->w) {
                int32_t s = x;
                while (x < f->w && row[x] == 0) x++;
                if (x >= f->w) break;
                while (s < x) { int32_t n = x - s; if (n > 0x7f) n = 0x7f; *out++ = (uint8_t)(n | 0x80); s += n; }
                s = x;
                while (x < f->w && row[x] != 0) x++;
                while (s < x) {
                    int32_t n = x - s; if (n > 0x7f) n = 0x7f;
                    *out++ = (uint8_t)n;
                    memcpy(out, row + s, (size_t)n);
                    out += n; s += n;
                }
            }
        }
        *out++ = 0x80;
        row += step;
    }
    uint32_t size; memcpy(&size, hdr + 0x1c, 4);
    if (out == hdr + size + 0x20) return 1;
    log("Sprite Decompression Error");
    return 0;
}
