// The browser's belt-toggle button hover plate takes its hit box, string id and plate position from the retail
// draw (d2client_267b0.cpp). The string lookup (string.tbl 0x1047 / 0x1048) stays with the caller.
#include <cstdint>
#if __has_include(<emscripten.h>)
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
extern "C" {
// 1.13c D2Client+0x267B0. expanded: the belt popup flag (0xFADD4). Returns the string index (0x1047 closed, 0x1048 open)
// when the pointer is on the button, else 0; out[0] = plate x, out[1] = plate y (the D2Win #10085 arguments).
EMSCRIPTEN_KEEPALIVE int32_t d2_client_belt_toggle_tip(int32_t w, int32_t h, int32_t mx, int32_t my, int32_t expanded, int32_t* out) {
    const int32_t w2 = (w - (w >> 31)) >> 1;
    if (mx < w2 - 8 || mx > w2 + 5) return 0;
    if (my < h - 0x27 || my > h - 0xd) return 0;
    out[0] = w2 - 1;
    out[1] = h - 0x27;
    return expanded ? 0x1048 : 0x1047;
}
}
