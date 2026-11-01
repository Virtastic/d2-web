// D2Game per-game unit sweep in the retail 32-bit layout (addresses are the low 32 bits of a pointer).
#include <cstdint>
#include <cstring>

static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

struct D2Game38860Env {
    void (*visit)(uint32_t game, uint32_t unit);   // 0xE20A0: ECX = game, EDX = unit
};

// 1.13c D2Game+0x38860
// EDI = game. The 128 hash buckets at game+0x1120 are walked in order; each chain is followed through
// unit+0xE4, the next link read before the visit, and every unit goes to 0xE20A0 with the game.
void GAME_VisitUnitBuckets_38860(uint32_t game, const D2Game38860Env& env)
{
    for (uint32_t i = 0; i < 0x80; ++i) {
        uint32_t u = rd32(game + 0x1120 + 4 * i);
        while (u != 0) {
            const uint32_t next = rd32(u + 0xE4);
            env.visit(game, u);
            u = next;
        }
    }
}
