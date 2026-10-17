#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x197D0: decision only; the caller performs the calls.
// p = ord10511() return, flags = ord10397(p) return, unit_c4 = *(u32*)(unit+0xC4) (ignored when !has_unit), flag = flag.
// Returns 0 = nothing, 1 = call 0xA3000, 2 = ord10137(p, flags|2) then 0xA3000.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_197d0(uint32_t p, uint32_t flags, uint32_t has_unit, uint32_t unit_c4, uint32_t flag) {
    if (!p) return 0;
    if (flags & 1) return 2;
    return (!flag && (!has_unit || !((unit_c4 >> 6) & 1))) ? 1 : 0;
}
