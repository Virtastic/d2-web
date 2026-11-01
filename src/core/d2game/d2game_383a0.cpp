// D2Game death-penalty dispatch in the retail 32-bit layout (addresses are the low 32 bits of a pointer).
#include <cstdint>
#include <cstring>

static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

struct D2Game383A0Env {
    void (*goldPenalty)(uint32_t unit, uint32_t game, uint32_t killer);              // 0x38180: ESI = unit; stack game, killer (ret 8)
    uint32_t (*findUnit)(uint32_t game, uint32_t id, uint32_t type);                 // 0xE03A0: ECX = game, EDX = id, EAX = type
    void (*loseExperience)(uint32_t game, uint32_t unit);                            // 0x37730: EAX = game; stack unit (ret 4)
};

// 1.13c D2Game+0x383A0
// ECX = unit, EAX = killer, EBX = game; plain ret. The gold penalty 0x38180 always runs first. A player
// killer (type 0) spares the experience loss, and so does a monster (type 1) whose monster data (+0x14)
// has an owner record (+0x28) naming a game (+0x28) in which the owner (type +0x30, id +0x2C) is a
// player. Every other case, a null killer included, goes on to 0x37730.
void PLAYER_DeathPenalties_383A0(uint32_t unit, uint32_t killer, uint32_t game, const D2Game383A0Env& env)
{
    env.goldPenalty(unit, game, killer);
    if (killer != 0) {
        const uint32_t type = rd32(killer);
        if (type == 0) return;
        if (type == 1) {
            const uint32_t md = rd32(killer + 0x14);
            const uint32_t owner = md ? rd32(md + 0x28) : 0;
            const uint32_t g = owner ? rd32(owner + 0x28) : 0;
            if (g != 0) {
                const uint32_t u = env.findUnit(g, rd32(owner + 0x2C), rd32(owner + 0x30));
                if (u != 0 && rd32(u) == 0) return;
            }
        }
    }
    env.loseExperience(game, unit);
}
