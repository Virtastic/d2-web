#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x119C0 (size 83, both fragments): out=0; ok = sub_3A270(entry=[[0x6FBA8B94]+[0x6FBCC538]*40],
// mod=[0x6FBA8BE8], key=caller AX, &out). ok != 0: sub_114F0(1) (returns 1 here, the caller makes that call).
// ok == 0 and out != 0 (3A270 stores 0xF8B on its reject path): [0x6FBCC54C] = out (word),
// [0x6FBCC550] = result of the IAT call at 0x6FB7EF5C (tick). Returns 0 then.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_119c0(uint32_t ok3a270, uint32_t out3a270, uint16_t* lastWord,
                                                         uint32_t* lastTick, uint32_t tick) {
    if (ok3a270 != 0) return 1;
    if ((uint16_t)out3a270 != 0) { *lastWord = (uint16_t)out3a270; *lastTick = tick; }
    return 0;
}
