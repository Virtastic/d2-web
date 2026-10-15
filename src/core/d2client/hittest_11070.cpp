#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x11070: hit box test. x=ESI, y=EDI, w=[0x6FB8BC48], h=[0x6FBA9E18].
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_11070(int32_t x, int32_t y, int32_t w, int32_t h) {
    int32_t m = (w - 0x26c) / 2, n = (h - 0x1a4) / 2;
    return x >= w - m - 0x1f && x <= w - m - 0x12 && y >= n + 0x125 && y <= n + 0x131;
}
