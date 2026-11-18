#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x1660: unit Y. pData (unit+0x2C) type 2/4/5: static path +0x10; else null -> 0, else D2Common ord 10750 (u16 at path+6).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1660(int32_t type, uint32_t pathPresent, uint32_t staticY, uint32_t dynY) {
    if (type == 2 || (type >= 4 && type <= 5)) return staticY;
    return pathPresent ? (dynY & 0xFFFFu) : 0;
}
