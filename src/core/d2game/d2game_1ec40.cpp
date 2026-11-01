// Decompile-derived, 1.13c-only. Seams: Fog pool free (D2Game+0xA2EC) and D2Common #10850 (D2Game+0xB34E).
#include <cstdint>
#include <cstring>

struct D2Game1ec40Env {
    void (*freePool)(uint32_t pool, uint32_t p, const char* file, int line);
    void (*freeAux)(uint32_t pool, uint32_t p);
};
static uint32_t r32(uint32_t base, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(base + o), 4); return v; }
static void w32(uint32_t base, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(base + o), &v, 4); }

// 1.13c D2Game+0x1EC40
// ECX = unit id (low 16 bits), AL = act index, EBX = pGame, plain ret. Walks the inactive lists at game+0xD8+act*4 (list next +0x14,
// node chain head +0xC, node next +0x58, node id +8), unlinks the matching node and frees its +0x20 chain, +0x24 block and itself.
extern "C" void d2game_remove_inactive_by_id_1ec40(uint32_t id, uint32_t act, uint32_t game, const D2Game1ec40Env* env)
{
    const uint32_t pool = r32(game, 0x1c);
    for (uint32_t list = r32(game, 0xd8 + (act & 0xff) * 4); list; list = r32(list, 0x14)) {
        uint32_t prev = 0;
        for (uint32_t n = r32(list, 0xc); n; prev = n, n = r32(n, 0x58)) {
            if (r32(n, 8) != (id & 0xffff)) continue;
            if (n == r32(list, 0xc)) w32(list, 0xc, r32(n, 0x58)); else w32(prev, 0x58, r32(n, 0x58));
            for (uint32_t it = r32(n, 0x20); it;) {
                const uint32_t nx = r32(it, 4);
                env->freePool(pool, it, "..\\Source\\D2Game\\Ai\\AiGeneral.cpp", 0x246);
                it = nx;
            }
            if (r32(n, 0x24)) env->freeAux(pool, r32(n, 0x24));
            env->freePool(pool, n, "..\\Source\\D2Game\\UNIT\\SUnitInactive.cpp", 0x8d);
            return;
        }
    }
}
