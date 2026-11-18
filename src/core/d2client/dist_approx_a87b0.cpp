#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0xA87B0 -- internal static, no export ordinal (unverified).
// Octagonal distance approximation: (max*0x3d7 + min*0x197) >> 10 of |a|,|b|.
// a arrives in EAX (usercall), b on the stack.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a87b0(int32_t a, int32_t b) {
    int32_t x = (a ^ (a >> 31)) - (a >> 31);
    int32_t y = (b ^ (b >> 31)) - (b >> 31);
    int32_t hi = y <= x ? x : y, lo = y <= x ? y : x;
    return static_cast<uint32_t>(hi * 0x3d7 + lo * 0x197) >> 10;
}
