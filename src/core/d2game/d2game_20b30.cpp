// Decompile-derived, 1.13c-only. Seam: Fog pool free (D2Game+0xA2EC).
#include <cstdint>
#include <cstring>

struct D2Game20b30Env { void (*freePool)(uint32_t pool, uint32_t p, const char* file, int line); };
static uint32_t r32(uint32_t base, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(base + o), 4); return v; }

// 1.13c D2Game+0x20B30
// EDX = chain head (next +4), EDI = pGame, plain ret. Frees every link (AiGeneral.cpp:0x246) through pool = game+0x1C, next link read first.
extern "C" void d2game_free_ai_chain_20b30(uint32_t head, uint32_t game, const D2Game20b30Env* env)
{
    for (uint32_t n = head; n;) {
        const uint32_t next = r32(n, 4);
        env->freePool(r32(game, 0x1c), n, "..\\Source\\D2Game\\Ai\\AiGeneral.cpp", 0x246);
        n = next;
    }
}
