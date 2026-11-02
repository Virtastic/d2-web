// D2Client batch b60-cli-d: panel-texture release, hover-state reset, rect hit test. Globals/callees via env seam.
#include <cstdint>
#include <initializer_list>
#include <emscripten.h>
#include "b60d_seam.h"
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
