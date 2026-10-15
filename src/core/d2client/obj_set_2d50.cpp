#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x2D50: EAX = obj, stack (a,b,c). If obj && obj[0]==0 && obj.inner(+0x14) != 0:
// inner+0x150 = 1, +0x154 = a, +0x158 = b, +0x15C = c. Browser has no 32-bit pointers: presence flags
// in, k (0..3) selects which of the four inner words is returned; untouched words read 0xA5A5A5A5.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2d50(uint32_t unitPresent, uint32_t first, uint32_t innerPresent,
                                                         uint32_t a, uint32_t b, uint32_t c, uint32_t k) {
    uint32_t w[4] = { 0xA5A5A5A5u, 0xA5A5A5A5u, 0xA5A5A5A5u, 0xA5A5A5A5u };
    if (unitPresent && first == 0 && innerPresent) { w[0] = 1; w[1] = a; w[2] = b; w[3] = c; }
    return w[k & 3];
}
