// 1.13c D2Win+0x17F40 (117 bytes): draw the cursor/tooltip cell. Globals + Ordinal_10041 via env seam.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Win17F40Env {
    int32_t  g_14dc, g_ffba0;       // DAT_6f9014dc, DAT_6f8ffba0 (gates)
    int32_t  mouseX, mouseY;        // DAT_6f901488 / 8c
    uint32_t g_14e8;                // DAT_6f9014e8
    int32_t  out_14e0, out_14e4;    // written: copies of mouse x/y
    void (*drawCell)(void* rec, int32_t x, int32_t y, uint32_t a, uint32_t b, void* c);  // Ordinal_10041
};

// 1.13c D2Win+0x17F40
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_17f40(D2Win17F40Env* e) {
    if (e->g_14dc == 0 || e->g_ffba0 == 0) return;
    e->out_14e0 = e->mouseX;
    e->out_14e4 = e->mouseY;
    const int32_t x = e->mouseX < 1 ? 0 : e->mouseX;
    const int32_t y = e->mouseY < 1 ? 0 : e->mouseY;
    uint32_t rec[18] = {0};
    rec[0] = e->g_14e8;
    rec[(0x48 - 0x14) / 4] = (uint32_t)e->g_14dc;
    e->drawCell(rec, x, y - 1, 0xffffffffu, 5, nullptr);
}
