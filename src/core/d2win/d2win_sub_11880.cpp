// 1.13c D2Win+0x11880 (106 B, __fastcall CL=idx): test bit (idx&31) of gdwBitMasks-style table word idx>>5.
// idx 0xFF -> 0; idx>>5 > 7 is fatal (Fog #10265(0x4D8) -> #10024 -> _exit(-1)). Table 0x6F8FC5C0 is passed in.
#include <cstdint>
#include <unistd.h>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_11880(uint8_t idx, const uint32_t* table) {
    if (idx == 0xFF) return 0;
    if ((idx >> 5) > 7) _exit(-1);
    return (table[idx >> 5] >> (idx & 31)) & 1;
}
