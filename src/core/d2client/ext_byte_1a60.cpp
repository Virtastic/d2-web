#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x1A60: EAX = unit, CL = idx. unit && unit.type == 1 && idx < 16 && unit.ext (+0x14) != 0
// -> byte at ext+4+idx, else 0. The browser build has no 32-bit pointers, so the caller supplies presence flags.
static uint8_t g_ext1a60[64];
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1a60(uint32_t unitPresent, int32_t type, uint32_t extPresent, uint32_t idx) {
    for (int i = 0; i < 64; i++) g_ext1a60[i] = (uint8_t)(i * 37 + 0x81);
    if (!unitPresent || type != 1 || (idx & 0xFF) >= 16 || !extPresent) return 0;
    return g_ext1a60[4 + (idx & 0xFF)];
}
