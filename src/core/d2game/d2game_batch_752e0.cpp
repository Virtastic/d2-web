// 0x752E0 table dispatch (sibling of 0x752A0, callback slot +4), from the cached 1.13c decompile. RE-DONE only.
#include <cstdint>
using i32 = int32_t;
#define W __attribute__((weak))
extern "C" { W i32* r_749A0(i32*); W void r_750F0(); }

// 1.13c D2Game+0x752E0 (fastcall: edx = param_2)
void D2GAME_752E0(i32, i32* param_2)
{
    i32* e = r_749A0(param_2);
    if (e && e[1]) ((void (*)())(intptr_t)e[1])();
    else r_750F0();
}
