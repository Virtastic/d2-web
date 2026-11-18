#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x11410 (no args): select the panel layout by flag [0x6FBC9854]. Writes ptr [0x6FBA8B94]
// (to 0x6FBA8928 if flag != 0 else 0x6FBA8728), G [0x6FBCC4FC], D [0x6FBCC500], X [0x6FBCC504].
// k: 0 ptr, 1 G, 2 D, 3 X.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_11410(uint32_t f, uint32_t k) {
    const uint32_t a[4] = { 0x6FBA8928u, 0x3E, 5, 0x2F }, z[4] = { 0x6FBA8728u, 0x33, 6, 0x24 };
    return (f ? a : z)[k & 3];
}
