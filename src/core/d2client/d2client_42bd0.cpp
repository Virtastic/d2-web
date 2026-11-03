// 1.13c D2Client+0x42BD0 -- [11C388] = 1; the word at [11B980]+0x1EB gets bits 10..12 := (byte [11C390] + 1) (mask 0xE0FF kept).
#include <cstdint>
#include <emscripten.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_42bd0(uint32_t* g_11c388, uint8_t g_11c390, uint8_t* player) {
    *g_11c388 = 1;
    uint16_t w = *(uint16_t*)(player + 0x1EB);
    *(uint16_t*)(player + 0x1EB) = (uint16_t)(((uint32_t)g_11c390 + 1) * 0x400 | (w & 0xE0FF));
}
