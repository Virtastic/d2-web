// D2Client+0x9CE80: EAX=node (u32 addr). Walks list DAT_6fbcbc14 (next +0x80), counts nodes whose u16 @+0x22 equals node's.
#include <cstdint>
#include <emscripten.h>
extern "C" {
// 1.13c D2Client+0x9CE80
EMSCRIPTEN_KEEPALIVE int16_t d2c_9ce80(uint32_t eax, uint32_t list_bc14) {
    auto r16 = [](uint32_t a) { return *(const int16_t*)(uintptr_t)a; };
    auto r32 = [](uint32_t a) { return *(const uint32_t*)(uintptr_t)a; };
    int32_t n = 0;
    if (eax && list_bc14) {
        for (uint32_t p = list_bc14; p; p = r32(p + 0x80)) if (r16(p + 0x22) == r16(eax + 0x22)) n++;
        if (n > 1) return r16(eax + 0x22);
    }
    return -1;
}
}
