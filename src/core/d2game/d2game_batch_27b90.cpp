// Implementation 27b90, from the cached 1.13c decompile. RE-DONE only, not yet linked or vectored.
#include <cstdint>
using i32 = int32_t; using u32 = uint32_t;
extern "C" {
intptr_t r_E07A0(...); intptr_t o10392(...); i32 o11133(void*);
void r_273C0(i32, i32*, i32); intptr_t r_DED30(...);
intptr_t r_A0C70(...);  // variadic: the batch 452c0b call passes more stack arguments (same wasm signature)
}

// 1.13c D2Game+0x27B90
extern "C" i32 D2GAME_27B90(i32 game, i32* unit)
{
    r_E07A0(game);
    i32* tgt = (i32*)(intptr_t)o10392(unit[0xb]);
    if (tgt == unit || !tgt) return 0;
    if (!((unit[0xe] >> 8) & 1)) { // bit 8 of unit flags clear
        r_273C0(game, unit, 0);
        r_DED30(game, unit, tgt);
        return 1;
    }
    unit[0x31] |= 0x40;
    r_A0C70(unit, nullptr, 0, o11133(unit) + 4, 0x2a783, "");
    return 0;
}
