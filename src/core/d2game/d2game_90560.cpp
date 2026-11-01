// 1.13c D2Game+0x90560 -- tell the proxy layer that a player left a tracked level.
// EAX = game (skip when byte +0x6A is 3), stack (ret 4) = unit. The unit's object (0xA478) gives a level id (0xA556); only ids
// 1, 40, 75 and 103 map to a mode 0..3 (every other id, and 0 or > 103, returns). The unit word's low byte is replaced by the
// mode (retail overwrites the stack slot). A pass over the living players (0xDFC10 with callback 0x6FCAF410) counts how many are
// on that level into {level, count}; then 0x90320(EBX = game, stack = word, count - 1 == 0) runs.
#include "d2game_8d6_common.h"
struct D2Game90560Env {
    uint32_t (*getObject)(uint32_t unit);                         // 0xA478
    uint32_t (*getLevel)(uint32_t obj);                           // 0xA556
    void (*countPlayers)(uint32_t game, uint32_t* levelAndCount); // 0xDFC10 + callback 0x6FCAF410
    void (*sub90320)(uint32_t game, uint32_t word, uint32_t last);// 0x90320
};
// 1.13c D2Game+0x90560
void d2game_port_90560(uint32_t game, uint32_t unit, const D2Game90560Env* env)
{
    if (d2g8::r8(game, 0x6A) == 3) return;
    const uint32_t obj = env->getObject(unit);
    if (!obj) return;
    const uint32_t lvl = env->getLevel(obj);
    uint32_t mode;
    switch (lvl) {
        case 1: mode = 0; break;
        case 40: mode = 1; break;
        case 75: mode = 2; break;
        case 103: mode = 3; break;
        default: return;
    }
    const uint32_t word = (unit & 0xFFFFFF00u) | mode;
    uint32_t st[2] = { lvl, 0 };
    env->countPlayers(game, st);
    env->sub90320(game, word, (st[1] - 1u) == 0 ? 1u : 0u);
}
