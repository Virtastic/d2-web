// The browser's mini-panel button hover takes its row box, column test and plate position from the retail mini-panel
// hit test (0x644F0, which calls the tooltip builder 0x63940). The tooltip text (string ids, key bindings) is still built by the caller.
#include <cstdint>
#if __has_include(<emscripten.h>)
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
extern "C" {
// 1.13c D2Client+0x644F0. mode: the panel-side word 0x644F0 receives in EAX (0 = no hit test, 1 = row shifted left by 0x76,
// 3 = shifted right by 0x77, else centred). left: the hovered button's left edge (retail table 0x6FBC54F0[i]); y of the plate row
// is the table's 0x6FBC5510[i] = H-0x32, drawn at y-0x1A, x = left-3 (the session-captured D2Win#10085 call).
// Returns 1 when the pointer is in the row box and strictly inside the button's 0x14 column, with out[0] = plate x, out[1] = plate y.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_mini_tip(int32_t w, int32_t h, int32_t mx, int32_t my, int32_t mode, int32_t left, int32_t* out) {
    if (mode == 0) return 0;
    int32_t c = mode == 1 ? -0x76 : mode == 3 ? 0x77 : 0;
    c += w / 2;
    if (!(c - 0x54 < mx && mx < c + 0x59)) return 0;
    if (!(h - 0x45 < my && my < h - 0x2f)) return 0;
    if (!(left < mx && mx < left + 0x14)) return 0;
    out[0] = left - 3;
    out[1] = h - 0x32 - 0x1a;
    return 1;
}
}
