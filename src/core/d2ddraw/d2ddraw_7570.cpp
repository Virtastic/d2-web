// src/core/d2ddraw/d2ddraw_7570.cpp -- 1.13c D2DDraw.dll ShadowTile. Decompile-derived.
// Same record walk as the lit tile. Each record inside the window calls the shadow row callback:
// slot +0x14 with colour 0xC0 when the mode flag is set, slot +4 otherwise.
// Retail returns EDI, which it sets to 1 only when a record draws; with no record drawn that register
// is not defined by the decompile. The port returns 0 in that case (see the spec's MISMATCH note).
#include <cstdint>

struct D2DDraw7570Env {
    uint32_t (*ord10106)(const uint8_t* ctx, uint32_t a, uint32_t b);      // ordinal 10106(ctx, 1, 0)
    void     (*shadowRow)(int32_t y);                                      // DAT_6f8d01dc+0x04
    void     (*shadowRowC0)(int32_t y, int32_t colour);                    // DAT_6f8d01dc+0x14
    int32_t  screenW, screenH;
    int32_t  colourFlag;                                                   // dword at DAT_6f8d01d0+0x14
};

// 1.13c D2DDraw+0x07570
// reg: ecx = ctx, esi = base x, stack: base y, mode. Returns 1 when any record drew.
uint32_t d2ddraw_shadow_tile(const D2DDraw7570Env* e, const uint8_t* ctx, const uint8_t* recs, int32_t count,
                             int32_t baseX, int32_t baseY, uint32_t mode) {
    if (e->ord10106(ctx, 1, 0) == 0) return 0;
    if (count < 1) return 0;
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
        drawn = 1;
        if (e->colourFlag == 0) e->shadowRow(vy);
        else e->shadowRowC0(vy, 0xC0);
    }
    return drawn;
}
