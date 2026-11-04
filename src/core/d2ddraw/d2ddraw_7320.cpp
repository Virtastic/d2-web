// src/core/d2ddraw/d2ddraw_7320.cpp -- 1.13c D2DDraw.dll TileDrawLit. Decompile-derived.
// Walks the tile record list and, for each record inside the clip window, calls the row callback
// (vtable slot +0x10) with the lit quad {b0, b1, b1, b0} read from the light table at index x>>5.
// The caller reads the record count (ctx+0x50) and list pointer (ctx+0x54); pointer width is the
// caller's. Each record is 0x14 bytes: int16 x at +0, int16 y at +2.
// Decompile artefacts: the retail mode (1 or 2) is read from the return-address slot; the port takes
// it as a parameter. The incoming local colour is overwritten before any read, so it has no input here.
#include <cstdint>

struct D2DDraw7320Env {
    uint32_t (*ord10106)(const uint8_t* ctx, uint32_t a, uint32_t b);      // ordinal 10106(ctx, 1, 0)
    void     (*drawRow)(int32_t y, const uint32_t* quad);                  // DAT_6f8d01dc+0x10
    int32_t  screenW, screenH;                                             // DAT_6f8d01c8, DAT_6f8d01d8
};

// 1.13c D2DDraw+0x07320
// reg: ecx = ctx, esi = base x, stack: base y, light table, mode. Returns 1 when any record drew.
uint32_t d2ddraw_tile_draw_lit(const D2DDraw7320Env* e, const uint8_t* ctx, const uint8_t* recs, int32_t count,
                               int32_t baseX, int32_t baseY, const uint8_t* lightTab, uint32_t mode) {
    if (e->ord10106(ctx, 1, 0) == 0) return 0;
    int32_t xLo = -0x20;
    int32_t xHi = e->screenW;
    if (mode == 1) {
        xHi = e->screenW - e->screenW / 2;
    } else if (mode == 2) {
        xLo = e->screenW / 2 - 0x20;
    }
    int32_t yHi = e->screenH + 0x20;
    uint32_t drawn = 0;
    for (int32_t i = 0; i < count; ++i) {
        const uint8_t* r = recs + i * 0x14;
        int16_t rx = (int16_t)(r[0] | (r[1] << 8));
        int16_t ry = (int16_t)(r[2] | (r[3] << 8));
        int32_t vx = rx + baseX;
        int32_t vy = ry + baseY;
        if (!(xLo <= vx && vx < xHi && -0x21 < vy && vy < yHi)) continue;
        int32_t idx = rx >> 5;
        uint32_t b0 = lightTab[idx * 4];
        uint32_t b1 = lightTab[idx * 4 + 4];
        uint32_t quad[4] = {b0, b1, b1, b0};
        e->drawRow(vy, quad);
        drawn = 1;
    }
    return drawn;
}
