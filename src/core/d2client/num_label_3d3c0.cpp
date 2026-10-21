// D2Client+0x3D3C0: draw a count (EBX) as text, trimmed to width 0x28; env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2c3D3C0Env {
    void (*itoaW)(uint16_t* out16, int32_t n);        // __itoa + Unicode::toUnicode (max 16)
    int32_t (*textWidth)(const uint16_t* s);          // Ordinal_10028
    void (*drawText)(const uint16_t* s, int32_t a, int32_t b, int32_t c);  // Ordinal_10150(param_2-1,3,0)
};
extern "C" {
// 1.13c D2Client+0x3D3C0
EMSCRIPTEN_KEEPALIVE void d2c_3d3c0(const D2c3D3C0Env* e, int32_t ebx, int32_t p2) {
    uint16_t t[32] = {};
    if (ebx <= 0) return;
    e->itoaW(t, ebx);
    int32_t i = 0; while (t[i]) i++;
    if (e->textWidth(t) > 0x28) {
        do { t[i--] = 0; } while (e->textWidth(t) > 0x28);
    }
    e->textWidth(t);
    e->drawText(t, p2 - 1, 3, 0);
}
}
