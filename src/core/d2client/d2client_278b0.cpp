// 1.13c D2Client+0x278B0: stamina bar and its hover text. Every callee goes through the injected dispatcher;
// the Unicode imports are keyed by their IAT slot RVA (0xCEB00 ctor, 0xCEAB4 sprintf).
#include "d2client_callee.h"
// 1.13c D2Client+0x278B0
extern "C" void d2c_278b0(const D2cSvc& v, uint32_t unit /*0x11BBFC*/, int32_t w /*0xDBC48*/, int32_t h /*0xDBC4C*/, int32_t mouseX /*0x11B828*/, int32_t mouseY /*0x11B824*/, uint8_t* pal /*0x11C4B8..BA*/) {
    int32_t cur = (int32_t)d2c_call(v, 0x26b20, 0xa, 0, 0, 0, 0);
    int32_t mx = (int32_t)d2c_call(v, 0xcf34, 0, 0, 0, 0, 0, unit);
    if (!pal[0]) {
        pal[0] = (uint8_t)d2c_call(v, 0xd390, 0, 0, 0, 0, 0, 0xff, 0, 0);
        pal[1] = (uint8_t)d2c_call(v, 0xd390, 0, 0, 0, 0, 0, 0xf4, 0xc0, 0x4c);
        pal[2] = (uint8_t)d2c_call(v, 0xd390, 0, 0, 0, 0, 0, 0, 0, 0xff);
    }
    uint32_t col = pal[1];
    if (mx + 5 < cur || d2c_call(v, 0xcbda, 0, 0, 0, 0, 0, unit, 0x18)) { col = pal[2]; mx = cur; }
    int32_t width;
    if (mx > 0) { width = (int32_t)((uint32_t)cur * 0x66u) / mx; if (width < 0x19) col = pal[0]; }
    else { width = 0; col = pal[0]; }
    int32_t w2 = (w - (w >> 31)) >> 1;
    int32_t x1 = w2 - 0x7f, y1 = h - 0x1b;
    d2c_call(v, 0xd0ba, 0, 0, 0, 0, 0, (uint32_t)x1, (uint32_t)y1, (uint32_t)(x1 + width), (uint32_t)(y1 + 0x12), col, 2);
    const int32_t c2 = (int32_t)d2c_call(v, 0x26b20, 0xa, 0, 0, 0, 0);
    int32_t es = (c2 + ((c2 >> 31) & 0xff)) >> 8;
    const int32_t m2 = (int32_t)d2c_call(v, 0xcf34, 0, 0, 0, 0, 0, unit);
    const int32_t ed = (m2 + ((m2 >> 31) & 0xff)) >> 8;
    uint32_t tcol = 0;
    if (es > ed || d2c_call(v, 0xc1fc, 0, 0, 0, 0, 0, unit, 0x88)) { es = ed; tcol = 3; }
    uint16_t buf[100];
    for (int i = 0; i < 100; ++i) d2c_call(v, 0xceb00, 0, (uint32_t)(uintptr_t)&buf[i], 0, 0, 0);   // Unicode ctor x100
    w2 = (w - (w >> 31)) >> 1;
    if (mouseX < w2 - 0x7f || mouseX > w2 - 0x19) return;
    if (mouseY < h - 0x1b || mouseY > h - 9) return;
    const uint32_t fmt = d2c_call(v, 0xd1ec, 0, 0x1044, 0, 0, 0, (uint32_t)es, (uint32_t)ed);
    d2c_call(v, 0xceab4, 0, 0, 0, 0, 0, 0x64, (uint32_t)(uintptr_t)buf, fmt, (uint32_t)es, (uint32_t)ed);   // Unicode::sprintf
    d2c_call(v, 0xd384, 0, (uint32_t)(uintptr_t)buf, (uint32_t)(w2 - 0x4c), 0, 0, (uint32_t)(h - 0x34), tcol, 1);
}
