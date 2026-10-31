// D2Client+0xAA290: panel-local band classifier; ESI = x, EDI = y. Returns band 0..4, or -1 for outside.
// Gate G = 0xBF8C() (called first); layout flag F = dword at D2Client+0x119854, read after the gate.
// Branch A (G and F nonzero): 0x40/0x80/0xC0/0x100 thresholds, bands 0..4. Branch B: 0x50/0xA0/0xF0, bands 0..3.
#include <cstdint>
#include <emscripten.h>

struct D2cBandEnv {
    int32_t (*gate0xBF8C)();      // D2Client+0xBF8C
    const int32_t* layoutF;       // &D2Client+0x119854
};

extern "C" {
// 1.13c D2Client+0xAA290
EMSCRIPTEN_KEEPALIVE int32_t d2c_aa290(const D2cBandEnv* e, int32_t x, int32_t y) {
    int32_t g = e->gate0xBF8C();
    int32_t f = g ? *e->layoutF : 0;
    if (y > 0x1E || x > 0x140) return -1;
    if (g && f) {
        if (x < 0x40) return 0;
        if (x < 0x80) return 1;
        if (x < 0xC0) return 2;
        return x >= 0x100 ? 4 : 3;
    }
    if (x < 0x50) return 0;
    if (x < 0xA0) return 1;
    return x >= 0xF0 ? 3 : 2;
}
}
