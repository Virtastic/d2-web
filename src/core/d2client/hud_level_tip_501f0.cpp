// The browser's lit level-up button hover plate takes its hit test and plate position from the retail hint-panel
// functions (800x600 mode, which the widescreen client keeps): the left button is 0x50430 (string 0xF92, hit 0x4FF10),
// the right is 0x501F0 (string 0xF93, hit 0x4FE70). The text is looked up by the caller.
#include <cstdint>
#if __has_include(<emscripten.h>)
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
extern "C" uint32_t d2_client_4ff10(int32_t w, int32_t h, int32_t mx, int32_t my);
extern "C" {
// 1.13c D2Client+0x4FE70: mouse inside the 22x34 rect right of screen centre.
static uint32_t in_4fe70(int32_t w, int32_t h, int32_t mx, int32_t my) {
    return (w / 2 + 0xa3 < mx && mx < w / 2 + 0xc5 && h - 0x2a < my && my < h - 8) ? 1 : 0;
}
// 1.13c D2Client+0x50430 / +0x501F0 (mode 2 text draw). Returns 0 (neither), 1 (stats, string 0xF92 = 3986) or
// 2 (skill, string 0xF93 = 3987); out[0] = plate x (W/2 -0xB3 / +0xB2), out[1] = plate y (H-0x32).
EMSCRIPTEN_KEEPALIVE int32_t d2_client_level_btn_tip(int32_t w, int32_t h, int32_t mx, int32_t my, int32_t* out) {
    out[1] = h - 0x32;
    if (d2_client_4ff10(w, h, mx, my)) { out[0] = w / 2 - 0xb3; return 1; }
    if (in_4fe70(w, h, mx, my)) { out[0] = w / 2 + 0xb2; return 2; }
    return 0;
}
}
