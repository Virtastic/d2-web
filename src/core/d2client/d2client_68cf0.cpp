// 1.13c D2Client+0x68CF0 (309 B, ESI = unit): the palette index of a unit's hit flash (drawn by 0x6B700 through D2Gfx #10046).
// On the first call it latches four colours through D2Win #10190 (the nearest entry of the module palette): green (0,0xC0,0),
// yellow (0xE0,0xE0,0), red (0xE0,0,0) and blue (0,0,0xC0) (DAT_6fbc54e8..eb, latch DAT_6fbcc050). A monster (type 1 from
// D2Common #10350) whose monstats row (0x1A8 bytes) has bit 4 of byte +0xD set answers blue; otherwise, with a non-zero maximum
// life (#10913), life (#10973 stat 6) * 100 / maximum below 0x21 answers red and below 0x42 yellow. Everything else is green.
#include <cstdint>

struct D2c68cf0Env {
    uint8_t (*ord10190)(uint8_t r, uint8_t g, uint8_t b);   // D2Win #10190
    uint8_t (*monFlags0d)(int32_t cls);                     // the monstats row's byte +0xD (0 outside the table)
    int32_t life, maxLife;                                  // #10973(unit, 6, 0), #10913(unit)
};

namespace { uint8_t g_col[4]; bool g_latched; }

// 1.13c D2Client+0x68CF0
extern "C" uint8_t d2c_68cf0(uint32_t type, int32_t cls, const D2c68cf0Env* e) {
    if (!g_latched) {
        g_col[0] = e->ord10190(0, 0xc0, 0);
        g_col[1] = e->ord10190(0xe0, 0xe0, 0);
        g_col[2] = e->ord10190(0xe0, 0, 0);
        g_col[3] = e->ord10190(0, 0, 0xc0);
        g_latched = true;
    }
    if (type != 1) return g_col[0];
    if (e->monFlags0d(cls) & 0x10) return g_col[3];
    if (e->maxLife != 0) {
        const int32_t pct = (e->life * 100) / e->maxLife;
        if (pct < 0x21) return g_col[2];
        if (pct < 0x42) return g_col[1];
    }
    return g_col[0];
}
