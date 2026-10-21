// Prior art: none vendored (1.13c-only AiGeneral tail). Decompile-derived from the cached 1.13c function.
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct D2Game21590Env {
    // 0xE03A0(game, id) (EAX = type): unit lookup.
    void* (*subE03A0)(uint32_t game, uint32_t id);
    // Ordinal_10046(file, line, 0): free; injected seam.
    void  (*free10046)(const char* file, uint32_t line, uint32_t zero);
};

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline uint8_t* P(uint32_t v) { return (uint8_t*)(uintptr_t)v; }

// 1.13c D2Game+0x21590
// unit (EAX). 32-bit layout offsets. Walks the list at ai+0x34, resetting each linked unit's ai
// (+0x2C=-1, +0x30=1, +0x28=game), then clears the list head. Null game is fatal (_exit -> abort).
extern "C" void d2_fn_d2game_21590(void* unit, const D2Game21590Env* env)
{
    if (!unit || rd32(unit, 0) != 1 || !rd32(unit, 0x14)) return;
    uint8_t* ai = P(rd32(P(rd32(unit, 0x14)), 0x28));
    if (!ai || !rd32(ai, 0x28)) return;
    void* u = env->subE03A0(rd32(ai, 0x28), rd32(ai, 0x2C));
    if (u != unit || rd32(u, 0) != 1) return;
    uint32_t game = rd32(ai, 0x28);
    for (uint32_t* n = (uint32_t*)P(rd32(ai, 0x34)); n;) {
        uint32_t* next = (uint32_t*)P(n[1]);
        if (!game) std::abort();
        for (uint8_t* m = P(rd32(P(game), 0x1320 + (n[0] & 0x7f) * 4)); m; m = P(rd32(m, 0xE4))) {
            if (rd32(m, 0xC) == n[0]) {
                if (rd32(m, 0) == 1) {
                    uint8_t* a = rd32(m, 0x14) ? P(rd32(P(rd32(m, 0x14)), 0x28)) : nullptr;
                    wr32(a, 0x2C, 0xffffffffu);
                    wr32(a, 0x30, 1);
                    wr32(a, 0x28, game);
                }
                break;
            }
        }
        env->free10046("..\\Source\\D2Game\\Ai\\AiGeneral.cpp", 0x237, 0);
        n = next;
    }
    wr32(ai, 0x34, 0);
}
