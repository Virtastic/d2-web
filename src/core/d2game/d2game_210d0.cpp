// 1.13c D2Game+0x210D0: walks the unit's list at +0x34 (node +0 key, +4 next), looks each key up in the game's
// 128-bucket unit hash (game+0x1320, chain link unit+0xE4, key unit+0xC), flags the matching monsters' AI
// structures and queues the first match with 0x20CF0. EAX = unit, ECX = other unit (may be null).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game210D0Env {
    void (*fatalNoGame)();                         // Ordinal 10265(0x2D3) + 10024 message, then _exit(-1)
    void (*queue)(uint32_t unit, uint32_t game, uint32_t edi);   // 0x20CF0: unit in EAX, game on the stack, EDI = other unit or the match
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }

// the AI block of a monster (unit+0x14 -> +0x28), or 0 when it has none
static uint32_t aiOf(uint32_t u) { const uint32_t p = rd32(u, 0x14); return p ? rd32(p, 0x28) : 0; }

static uint32_t findUnit(uint32_t game, uint32_t key)
{
    for (uint32_t u = rd32(game, 0x1320 + (key & 0x7f) * 4); u; u = rd32(u, 0xE4))
        if (rd32(u, 0xC) == key) return u;
    return 0;
}

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_game_210D0(uint32_t unit, uint32_t other, const D2Game210D0Env* e)
{
    const uint32_t head = rd32(unit, 0x34);
    if (!head) return;
    const uint32_t game = rd32(unit, 0x28);
    uint32_t node = head, first = 0;
    for (;; node = rd32(node, 4)) {
        if (!game) e->fatalNoGame();
        first = findUnit(game, rd32(node, 0));
        if (first) break;
        if (!rd32(node, 4)) return;
    }
    const uint32_t key0 = rd32(node, 0);   // the matched node's key is what every write below stores
    if (rd32(first, 0) == 1) {
        const uint32_t ai = aiOf(first);
        wr32(ai, 8, rd32(ai, 8) | 3);
        wr32(ai, 0x2C, key0); wr32(ai, 0x30, 1); wr32(ai, 0x28, game);
    }
    if (other && rd32(other, 0) == 1) {
        const uint32_t ai = aiOf(other);
        wr32(ai, 0x2C, key0); wr32(ai, 0x30, 1); wr32(ai, 0x28, game);
    }
    e->queue(first, game, other);
    // second pass restarts at the list head and skips the unit already handled
    for (node = head; node; node = rd32(node, 4)) {
        const uint32_t u = findUnit(game, rd32(node, 0));
        if (u && u != first) {
            if (rd32(u, 0) == 1) {
                const uint32_t ai = aiOf(u);
                wr32(ai, 0x2C, key0); wr32(ai, 0x30, 1); wr32(ai, 0x28, game);
            }
            e->queue(first, game, u);
        }
    }
}
