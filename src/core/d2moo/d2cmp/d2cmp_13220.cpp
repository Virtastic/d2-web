// D2CMP Codec.cpp: bounding box of one DCC direction's frames.
#include <cstdint>
#include <cstring>
#include "d2cmp_dcc.h"

// 1.13c D2CMP+0x13220
// Retail: ECX = header, EDX = direction, EAX = frames, EDI = box. dir >= header+8 -> fatal 0x373.
// Top edge: yoff - h for a top-down frame (bottomUp == 0), yoff for a bottom-up one.
// No frames leaves min 0x7FFFFFFF / max 0x80000000, so w and h wrap to 1 (32-bit arithmetic).
// Layout is read and written at the retail offsets (frame stride 0x2C, box dwords +0 hdr, +4 frames, +8 xmin, +C ymin,
// +10 w, +14 h, +18 dir, +1C nframes). The native DccFrame / DccBox shapes are 64-bit wide on the wasm64 build, so they are not used here.
static inline int32_t rd32(const uint8_t* p) { int32_t v; memcpy(&v, p, 4); return v; }
static inline void wr32(uint8_t* p, int32_t v) { memcpy(p, &v, 4); }

void d2cmp_13220(const int32_t* hdr, int32_t dir, DccFrame* frames, DccBox* box, D2CmpFatalFn fatal) {
    if (!(dir < hdr[2])) { fatal(0x373); return; }
    uint8_t* b = (uint8_t*)box;
    const uint8_t* fb = (const uint8_t*)frames;
    wr32(b + 0x00, (int32_t)(uintptr_t)hdr);
    wr32(b + 0x04, (int32_t)(uintptr_t)frames);
    wr32(b + 0x18, dir);
    wr32(b + 0x1C, hdr[3]);
    int32_t x0 = 0x7fffffff, x1 = (int32_t)0x80000000u, y0 = 0x7fffffff, y1 = (int32_t)0x80000000u;
    for (int32_t i = 0; i < hdr[3]; i++) {
        const uint8_t* f = fb + (size_t)i * 0x2C;
        const int32_t w = rd32(f + 0x04), h = rd32(f + 0x08), xoff = rd32(f + 0x0C), yoff = rd32(f + 0x10);
        const int32_t bottomUp = rd32(f + 0x28);
        const int32_t y = yoff - (bottomUp == 0 ? h : 0);
        if (xoff < x0) x0 = xoff;
        if (x1 < xoff + w) x1 = xoff + w;
        if (y < y0) y0 = y;
        if (y1 < y + h) y1 = y + h;
    }
    wr32(b + 0x14, (int32_t)((uint32_t)y1 - (uint32_t)y0));
    wr32(b + 0x08, x0);
    wr32(b + 0x0C, y0);
    wr32(b + 0x10, (int32_t)((uint32_t)x1 - (uint32_t)x0));
}
