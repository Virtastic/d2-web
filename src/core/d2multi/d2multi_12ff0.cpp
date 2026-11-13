// 1.13c D2Multi+0x12FF0 -- stdcall, one ignored argument: with the mode word at image offset 0x39DAC equal to 2, three D2Win 10166 strings (the controls at 0x39D80, 0x39D84, 0x39D88) are narrowed by D2Lang unicode2Win into locals of 8, 0x28 and 0xC8 bytes and handed, as an array of three pointers, to slot +0x74 of the interface table at 0x3917C (ECX = the record at 0x39F14 plus 0x89, EDX = 3, the table at 0x19C28 and the array on the stack); a zero answer returns at once, otherwise (and for any other mode) the worker 0x11510 runs. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_39dac;
__attribute__((weak)) uint32_t g_d2multi_39d80;
__attribute__((weak)) uint32_t g_d2multi_39d84;
__attribute__((weak)) uint32_t g_d2multi_39d88;
__attribute__((weak)) uint32_t g_d2multi_39f14;
namespace d2multi {
uint32_t f12ff0(GE& e, uint32_t)
{
    if (g_d2multi_39dac == 2) {
        const uint32_t s0 = e.h(0x7c50, 0, g_d2multi_39d80);
        const uint32_t s1 = e.h(0x7c50, 0, g_d2multi_39d84);
        const uint32_t s2 = e.h(0x7c50, 0, g_d2multi_39d88);
        uint8_t b0[8] = {}, b1[0x28] = {}, b2[0xc8] = {};
        e.h(0x16040, 0, e.stk(b0), s0, 0, 0, 8);
        e.h(0x16040, 0, e.stk(b1), s1, 0, 0, 0x28);
        e.h(0x16040, 0, e.stk(b2), s2, 0, 0, 0xc8);
        uint32_t arr[3] = { e.stk(b0), e.stk(b1), e.stk(b2) };
        // slot +0x74 of the table at [0x3917C]: the capture binds it to the thunk 0x7C02
        if (!e.h(0x7c02, 0, g_d2multi_39f14 + 0x89, 3, 0, 0, 0xB2019c28u, e.stk(arr))) return 1;   // je 0x6F9E30A0: a zero answer returns
    }
    e.h(0x11510);
    return 1;
}
}
