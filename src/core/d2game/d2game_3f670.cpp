// The Baal portal missile's do-function on retail's 32-bit layout (the vendored typed form is
// MISSMODE_SrvDo36_BaalFxControl in d2moo/src/D2Game/MISSILES/MissMode.cpp). ECX = game, EDX = missile, plain ret.
// While the missile's target-x flag is clear and its frame is <= 100: set the flag, and in the missile's room spawn Tyrael when
// quest 0x24 is past its intro, then mark the room as holding a portal. Always ends in the missile collision handler, whose
// result it returns.
#include <cstdint>

struct D2G3F670Env {
    int32_t  (*frame)(uint32_t missile);
    uint32_t (*targetX)(uint32_t missile);
    void     (*setTargetX)(uint32_t missile, uint32_t v);
    uint32_t (*room)(uint32_t missile);
    uint32_t (*notIntro)(uint32_t game, uint32_t quest);
    void     (*spawnTyrael)(uint32_t missile, uint32_t game, uint32_t room);
    void     (*portalFlag)(uint32_t room, uint32_t v);
    uint32_t (*collide)(uint32_t game);
};

uint32_t MISSMODE_BaalFxControl32(uint32_t game, uint32_t missile, const D2G3F670Env& env)
{
    const int32_t frame = env.frame(missile);
    if (!env.targetX(missile) && frame <= 100) {
        env.setTargetX(missile, 1);
        const uint32_t room = env.room(missile);
        if (env.notIntro(game, 0x24)) env.spawnTyrael(missile, game, room);
        env.portalFlag(room, 1);
    }
    return env.collide(game);
}
