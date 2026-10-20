// D2Game 1.13c sub_68D20 (batch, not compiled against a test runner). Rule over an environment E as in d2game_batch_5d5c0.cpp.
// REG?: ESI = obj (unaff_ESI; obj[1]=pGame, obj[6]=state) and the stack word param_1 are inputs; the argument
// order/registers of callees 65E90 and 8EA80 (its 7 args follow d2game_batch_5d5c0.cpp) are unverified.
#include <cstdint>

namespace b68d20 {
typedef uint32_t u32;

// 1.13c D2Game+0x68D20
template <class E> void Run(E& e, u32 esi /*REG?*/, u32 param1)
{
    u32 state = e.rd32(esi + 0x18);
    e.h(0x65E90, esi, state);
    state = e.rd32(esi + 0x18);
    if (e.rd8(state + 0x118) != 0 && e.rd8(state + 0x119) == 0) {
        u32 g = e.rd32(esi + 4);
        e.h(0x8EA80, g, param1, 7, e.rd32(g + 0xA8) + 10, 0, 0, 0);
    }
}
}
