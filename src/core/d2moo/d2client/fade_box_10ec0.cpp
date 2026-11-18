// 1.13c D2Client+0x10EC0 --
// Computes the D2Gfx #10045 argument tuple a[0..5], a[6] = deadline global after; returns 0 if no draw (deadline 0 or expired).
#include <emscripten.h>
#include <cstdint>
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_10ec0_box(uint32_t deadline, uint32_t now, int32_t w, int32_t h,
                                                            int32_t layout, int32_t* a) {
    a[6] = deadline;   // the global after the call: an expired deadline is cleared
    if (!deadline || deadline < now) { a[6] = 0; return 0; }
    int32_t x = 0, bw = w;
    if (layout == 1) bw = w - w / 2;
    else if (layout == 2) x = w / 2;
    a[0] = x; a[1] = 0; a[2] = bw; a[3] = h - 0x2f; a[4] = 0;
    a[5] = (int32_t)((deadline - now) * 0xffu) / 500;
    return 1;
}
