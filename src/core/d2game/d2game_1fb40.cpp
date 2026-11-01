// Decompile-derived, 1.13c-only. Seams: the per-node callback (D2Game+0x1F9F0) and the Fog pool free (D2Game+0xA2EC).
#include <cstdint>
#include <cstring>

struct D2Game1fb40Env {
    void (*node)(uint32_t game, uint32_t arg, uint32_t n);
    void (*freePool)(uint32_t pool, uint32_t p, const char* file, int line);
};
static uint32_t r32(uint32_t base, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(base + o), 4); return v; }

// 1.13c D2Game+0x1FB40
// EAX = list head (node next +0, expiry frame +4), EBX = pGame, stack arg (ret 4). A node with no expiry, or one at or past game+0xA8,
// goes to 0x1F9F0(game, arg, node); every node is then freed (SUnitInactive.cpp:0x127) through pool = game+0x1C.
extern "C" void d2game_flush_inactive_list_1fb40(uint32_t list, uint32_t game, uint32_t arg, const D2Game1fb40Env* env)
{
    for (uint32_t n = list; n;) {
        const uint32_t next = r32(n, 0), exp = r32(n, 4);
        if (exp == 0 || (int32_t)r32(game, 0xa8) <= (int32_t)exp) env->node(game, arg, n);
        env->freePool(r32(game, 0x1c), n, "..\\Source\\D2Game\\UNIT\\SUnitInactive.cpp", 0x127);
        n = next;
    }
}
