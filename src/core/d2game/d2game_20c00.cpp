// Decompile-derived, 1.13c-only. Seams: Fog pool free (D2Game+0xA2EC), D2Common #10850 (D2Game+0xB34E) and the command-list drain (D2Game+0x20B60).
#include <cstdint>
#include <cstring>

struct D2Game20c00Env {
    void (*freePool)(uint32_t pool, uint32_t p, const char* file, int line);
    void (*freeAux)(uint32_t pool, uint32_t p);
    void (*drain)(uint32_t ai, uint32_t game);    // 0x20B60: ESI = ai, EDI = game
};
static uint32_t r32(uint32_t base, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(base + o), 4); return v; }

// 1.13c D2Game+0x20C00
// EAX = AI control block, ECX = pGame, plain ret. Frees the +0x38 block, the +0x34 chain (next +4, AiGeneral.cpp:0x246), drains the command
// list and frees the block itself (AiGeneral.cpp:0x19D), all through pool = game+0x1C.
extern "C" void d2game_free_ai_control_20c00(uint32_t ai, uint32_t game, const D2Game20c00Env* env)
{
    if (!ai) return;
    static const char kFile[] = "..\\Source\\D2Game\\Ai\\AiGeneral.cpp";
    if (r32(ai, 0x38)) env->freeAux(r32(game, 0x1c), r32(ai, 0x38));
    for (uint32_t n = r32(ai, 0x34); n;) {
        const uint32_t next = r32(n, 4);
        env->freePool(r32(game, 0x1c), n, kFile, 0x246);
        n = next;
    }
    env->drain(ai, game);
    env->freePool(r32(game, 0x1c), ai, kFile, 0x19d);
}
