#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x11240: g = [0x6FBCC4FC] minus 15, or 0 if g-i <= 0 (signed) for any i in 1..15.
// The (hudH-0x1A4)/2 it also computes is dead (never used). hudH is [0x6FBA9E18].
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_11240(int32_t g, int32_t hudH) {
    (void)hudH;
    for (int i = 0; i < 15; i++) { g = (int32_t)((uint32_t)g - 1); if (g <= 0) return 0; }
    return g;
}
