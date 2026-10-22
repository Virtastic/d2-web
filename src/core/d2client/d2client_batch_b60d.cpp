// D2Client batch b60-cli-d: panel-texture release, hover-state reset, rect hit test. Globals/callees via env seam.
#include <cstdint>
#include <initializer_list>
#include <emscripten.h>
struct B60dEnv {
    int32_t *g_bb35c /*1BBB35C*/, *g_cbc6c /*1CBC6C*/, *g_cbc70 /*1CBC70*/, *g_cbc80 /*1CBC80*/;
    const int32_t *g_c9854 /*1C9854*/;
    void (*o10065)(int32_t); void (*o10043)(int32_t, int32_t);
    uint32_t *hover[6] /*B90EA8..B90EBC*/; uint32_t *z[4] /*1CBC28,2C,38,3C*/;
    const int32_t *gW /*8BC48*/, *gH /*8BC4C*/, *offX /*1CB9A0*/, *offY /*1CB9A4*/;
};
extern "C" {
// 1.13c D2Client+0x8B840
EMSCRIPTEN_KEEPALIVE void d2c_8b840(const B60dEnv* e) {
    for (int32_t* p : {e->g_bb35c, e->g_cbc6c, e->g_cbc70}) {
        int32_t v = *p;
        if (v) { e->o10065(v); e->o10043(0x7a, 0); *p = 0; }
    }
    int32_t v = *e->g_cbc80;
    if (*e->g_c9854 && v) { e->o10065(v); e->o10043(0x7a, 0); *e->g_cbc80 = 0; }
}
// 1.13c D2Client+0x8BBF0
EMSCRIPTEN_KEEPALIVE void d2c_8bbf0(const B60dEnv* e) {
    for (auto p : e->hover) *p = 0xffffffffu;
    for (auto p : e->z) *p = 0;
}
// 1.13c D2Client+0x8BD90  (ECX = x, EDX = y)
EMSCRIPTEN_KEEPALIVE int32_t d2c_8bd90(const B60dEnv* e, int32_t x, int32_t y) {
    int32_t bx = *e->gW - *e->offX, by = *e->offY + *e->gH;
    return bx - 0x12e <= x && x <= bx - 0x10e && by - 0x60 <= y && y <= by - 0x40;
}
}
