// 1.13c D2Game+0x6F880, injectable callees. ESI = missile unit, stack word 0 = game (ret 4). A non-null unit is released through
// thunk 0xB0E4 (D2Common #10178, 1 word) and then 0x8E880 (EAX = game, 1 stack word = unit, ret 4).
#include <cstdint>
struct G6f880Env {
    void (*b0e4)(uint32_t unit);
    void (*e880)(uint32_t eaxGame, uint32_t unit);
};
// 1.13c D2Game+0x6F880
void g_6f880(uint32_t esi, uint32_t game, const G6f880Env& env)
{
    if (esi == 0) return;
    env.b0e4(esi);
    env.e880(game, esi);
}
