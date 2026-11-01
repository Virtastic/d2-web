// Decompile-derived, 1.13c-only. Seams: Fog pool free (D2Game+0xA2EC) and the chain free (D2Game+0x1EBD0).
#include <cstdint>
#include <cstring>

struct D2Game1ed80Env {
    void (*freePool)(uint32_t pool, uint32_t p, const char* file, int line);
    void (*freeChain)(uint32_t node, uint32_t game);
};
static uint32_t r32(uint32_t base, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(base + o), 4); return v; }

// 1.13c D2Game+0x1ED80
// EAX = pGame, plain ret. For each of the 5 inactive lists at game+0xD8: free the node's +0x10 chain (next +0x30, line 0x29B) and +8 chain
// (next +0, line 0x2A5), run the 0x1EBD0 chain free with EAX = node and EDI = game, then free the node (line 0x2AC); pool = game+0x1C.
extern "C" void d2game_free_inactive_unit_lists_1ed80(uint32_t game, const D2Game1ed80Env* env)
{
    static const char kFile[] = "..\\Source\\D2Game\\UNIT\\SUnitInactive.cpp";
    for (uint32_t slot = 0; slot < 5; ++slot) {
        for (uint32_t n = r32(game, 0xd8 + slot * 4); n;) {
            const uint32_t next = r32(n, 0x14);
            for (uint32_t i = r32(n, 0x10); i;) { const uint32_t nx = r32(i, 0x30); env->freePool(r32(game, 0x1c), i, kFile, 0x29b); i = nx; }
            for (uint32_t i = r32(n, 8); i;) { const uint32_t nx = r32(i, 0); env->freePool(r32(game, 0x1c), i, kFile, 0x2a5); i = nx; }
            env->freeChain(n, game);
            env->freePool(r32(game, 0x1c), n, kFile, 0x2ac);
            n = next;
        }
    }
}
