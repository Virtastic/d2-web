// Level warp check: level 0x49 -> 0xC1A60, 0x64 -> 0xE2900(param), 0x76/0x80 with target 0x78 -> 0x56600, 0x84 ->
// 0x56570, otherwise 0. Each callee takes EAX = game.
#include <cstdint>

struct D2G34020Env {
    uint32_t (*c1a60)(uint32_t game);
    uint32_t (*e2900)(uint32_t game, uint32_t param);
    uint32_t (*c56600)(uint32_t game);
    uint32_t (*c56570)(uint32_t game);
};

// 1.13c D2Game+0x34020
uint32_t QUESTS_LevelWarpAllowed(uint32_t game, uint32_t level, uint32_t param, uint32_t target, const D2G34020Env& env)
{
    switch (level) {
    case 0x49: return env.c1a60(game);
    case 0x64: return env.e2900(game, param);
    case 0x76:
    case 0x80: return target == 0x78 ? env.c56600(game) : 0;
    case 0x84: return env.c56570(game);
    default: return 0;
    }
}
