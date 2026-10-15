#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x1630: unit X. pData (unit+0x2C) type 2/4/5: static path +0xC; else null -> 0, else D2Common sub_34BB0 (u16 at +2).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1630(int32_t type, uint32_t pathPresent, uint32_t staticX, uint32_t dynX) {
    if (type == 2 || (type >= 4 && type <= 5)) return staticX;
    return pathPresent ? (dynX & 0xFFFFu) : 0;
}
