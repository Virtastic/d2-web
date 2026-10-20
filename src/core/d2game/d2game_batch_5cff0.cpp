// Implementation batch 5cff0: written from the cached 1.13c decompile. Not yet linked or vectored (RE-DONE only).
#include <cstdint>
#include <cstdlib>
using i32 = int32_t; using u32 = uint32_t;
extern "C" {
i32 o10920(i32*); i32 o10331(i32); i32 o10826(i32); void r_386F0(); i32 o10265(i32); void o10024(const void*, i32);
}

// 1.13c D2Game+0x5CFF0 (fastcall: ecx = game, edx = unit)
i32 D2GAME_5CFF0(i32 game, i32 unit) // REG? ecx/edx assumed from __fastcall; r_386F0 takes no visible args (REG? none shown)
{
    i32 code;
    if (unit == 0) code = 0x24c;
    else {
        i32 p = o10920((i32*)(intptr_t)unit);
        if (p == 0) code = 0x24f;
        else if (game == 0) code = 0x2d3;
        else {
            u32 key = *(u32*)(intptr_t)(p + 0x48);
            i32 n = *(i32*)(intptr_t)(game + 0x1520 + (key & 0x7f) * 4);
            if (n == 0) return 0;
            while (*(u32*)(intptr_t)(n + 0xc) != key) {
                n = *(i32*)(intptr_t)(n + 0xe4);
                if (n == 0) return 0;
            }
            if (o10826(o10331(n)) == 0x78) r_386F0();
            return 0;
        }
    }
    o10024((void*)0, o10265(code)); // DAT_6fd19ff9 message arg
    std::_Exit(-1);
}
