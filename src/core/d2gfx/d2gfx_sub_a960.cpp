// 1.13c D2Gfx+0x0A960: walk a tile's 20-byte cell list, clip each cell, pick a draw routine.
#include <cstdint>
#include <cstring>

struct D2GfxA960State {
    int32_t clipTop;       // D2Gfx+0x9D460
    int32_t clipBottom;    // D2Gfx+0x9D468
    int32_t clipLeft;      // D2Gfx+0x948B4
    int32_t clipRight;     // D2Gfx+0x948C4
    int32_t litTop;        // D2Gfx+0x948AC  (unlit/fast path rect, y in [litTop,litBottom))
    int32_t litBottom;     // D2Gfx+0x948C8
    int32_t litLeft;       // D2Gfx+0x95440  (x in [litLeft,litRight))
    int32_t litRight;      // D2Gfx+0x9D450
    int32_t pitch;         // D2Gfx+0x9477C
    int32_t forceShade;    // D2Gfx+0x9D660
    int32_t alwaysShade;   // D2Gfx+0x90BE8
    uint8_t* surface;      // D2Gfx+0x94778 (destination origin)
};

struct D2GfxA960Ops {  // the four callees, injected; args as retail passes them
    void (*plain)(uint8_t* dst, uint32_t sh);                            // +0x9620 (sh is its stack word)
    void (*lightFast)(uint8_t* dst, const uint8_t* cell);                 // +0x95C0
    void (*shade)(uint8_t* dst, const uint8_t* cell, int32_t x, int32_t y, uint32_t sh);  // +0xA760
    void (*gouraud)(int32_t y, const uint8_t* cell, int32_t x, uint8_t* dst);             // +0xA5D0
};

// tileList: +0x50 int32 count, cells = the array behind +0x54 (20 bytes each:
// +0 i16 dx, +2 i16 dy, +6 u8 gx, +7 u8 gy, +8 u8 flags). light = param_4 (12-byte entries,
// RGB-ish corners at +0x6c/+0x78/+0xcc/+0xd8).
extern "C" void d2gfx_sub_a960(int32_t count, const uint8_t* cells, int32_t px, int32_t py,
                               const uint8_t* light, const D2GfxA960State* st,
                               const D2GfxA960Ops* ops) {
    uint8_t* base = st->surface + static_cast<int64_t>(st->pitch) * py;
    for (int32_t i = 0; i < count; ++i) {
        const uint8_t* c = cells + i * 0x14;
        int16_t dx, dy;
        std::memcpy(&dx, c, 2);
        std::memcpy(&dy, c + 2, 2);
        if (!(c[8] & 1)) continue;
        int32_t x = dx + px, y = dy + py;
        uint8_t* dst = base + static_cast<int64_t>(dy) * st->pitch + dx + px;
        if (!(st->clipTop <= y && y < st->clipBottom && st->clipLeft <= x && x < st->clipRight))
            continue;
        const int32_t k = c[6] + c[7] * 8;
        bool shade;
        uint32_t sh = 0;
        if (st->forceShade == 0) {
            const int32_t a = light[k * 12 + 0x78], b = light[k * 12 + 0x6c];
            const int32_t d1 = a - b < 0 ? b - a : a - b;
            int32_t d2 = light[k * 12 + 0xcc] - b; if (d2 < 0) d2 = -d2;
            int32_t d3 = light[k * 12 + 0xd8] - a; if (d3 < 0) d3 = -d3;
            if (st->alwaysShade != 0 || d1 + d2 + d3 < 10) { shade = true; sh = b; }
            else shade = false;
        } else {
            shade = true;
            sh = light[(c[6] + 9 + c[7] * 8) * 12];
        }
        const bool outside = y < st->litTop || st->litBottom <= y || x < st->litLeft || st->litRight <= x;
        if (outside) {
            if (shade) ops->shade(dst, c, x, y, sh);
            else ops->gouraud(y, c, x, dst);
        } else if (shade) ops->plain(dst, sh);
        else ops->lightFast(dst, c);
    }
}
