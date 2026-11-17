// The browser's experience and stamina bar hover plates take their mouse boxes, plate position and ink from the
// retail bar draws (d2client_26970.cpp, d2client_278b0.cpp) instead of JavaScript rectangles. The string text
// (string.tbl 4163 / 4164) is still formatted by the caller.
#include <cstdint>
#if __has_include(<emscripten.h>)
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" {
// 1.13c D2Client+0x26970 (experience) and D2Client+0x278B0 (stamina).
// w, h: screen size; mouseX/mouseY: pointer; cur/max: the stamina stat in whole points (stat >> 8);
// over: the player has state 0x88. Returns 0 (off both bars), 1 (experience) or 2 (stamina) and fills
// out[0] = plate x, out[1] = plate y, out[2] = ink (3 when the stamina is over its maximum or state 0x88),
// out[3] = the stamina value to print (clamped to max in that case).
EMSCRIPTEN_KEEPALIVE int32_t d2_client_hud_bar_tip(int32_t w, int32_t h, int32_t mouseX, int32_t mouseY,
                                                   int32_t cur, int32_t max, int32_t over, int32_t* out) {
    const int32_t w2 = (w - (w >> 31)) >> 1;
    out[2] = 0;
    out[3] = cur;
    if (mouseX >= w2 - 0x92 && mouseX <= w2 - 0x17 && mouseY >= h - 0x2b && mouseY <= h - 0x22) {
        out[0] = w2 - 0x92;
        out[1] = h - 0x33;
        return 1;
    }
    if (mouseX >= w2 - 0x7f && mouseX <= w2 - 0x19 && mouseY >= h - 0x1b && mouseY <= h - 9) {
        if (cur > max || over) { out[3] = max; out[2] = 3; }
        out[0] = w2 - 0x4c;
        out[1] = h - 0x34;
        return 2;
    }
    return 0;
}
}
