// Decompile-derived, 1.13c-only. Seams: the unit-by-id lookup (D2Game+0xE03A0) and the Fog pool free (+0xA2EC).
#include <cstdint>
#include <cstring>

struct D2Game21590Env {
    uint32_t (*lookup)(uint32_t game, uint32_t id, uint32_t type);                                 // 0xE03A0: ECX = game, EDX = id, EAX = type
    void (*freePool)(uint32_t pool, uint32_t p, const char* file, int line);                       // 0xA2EC: ECX = pool, EDX = block
};
static uint32_t r32(uint32_t base, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(base + o), 4); return v; }
static void w32(uint32_t base, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(base + o), &v, 4); }

// 1.13c D2Game+0x21590
// EAX = player unit. Re-finds the unit through its party block's (game, id, type) triple; if it is the same unit, walks the party's member list (block +0x34: id +0,
// next +4): every member found in the game's unit hash is reset (block +0x2C = -1, +0x30 = 1, +0x28 = game) and each list node is freed; the list head is cleared.
// Null game with a non-empty list: fatal exit (not vectored).
extern "C" void d2game_release_party_21590(uint32_t unit, const D2Game21590Env* env)
{
    if (!unit || r32(unit, 0) != 1) return;
    uint32_t pd = r32(unit, 0x14);
    if (!pd) return;
    uint32_t blk = r32(pd, 0x28);
    if (!blk) return;
    uint32_t game = r32(blk, 0x28);
    if (!game) return;
    const uint32_t found = env->lookup(game, r32(blk, 0x2c), r32(blk, 0x30));
    if (!found || found != unit || r32(found, 0) != 1) return;
    pd = r32(found, 0x14);
    blk = pd ? r32(pd, 0x28) : 0;
    uint32_t e = r32(blk, 0x34);
    const uint32_t g = r32(blk, 0x28);
    while (e) {
        const uint32_t next = r32(e, 4), id = r32(e, 0);
        if (!g) return;   // fatal exit in retail
        for (uint32_t n = r32(g, 0x1320 + (id & 0x7f) * 4); n; n = r32(n, 0xe4)) {
            if (r32(n, 0xc) != id) continue;
            if (r32(n, 0) == 1) {
                const uint32_t p2 = r32(n, 0x14);
                const uint32_t b2 = p2 ? r32(p2, 0x28) : 0;
                w32(b2, 0x2c, 0xffffffffu); w32(b2, 0x30, 1); w32(b2, 0x28, g);
            }
            break;
        }
        env->freePool(r32(g, 0x1c), e, "..\\Source\\D2Game\\Ai\\AiGeneral.cpp", 0x237);
        e = next;
    }
    w32(blk, 0x34, 0);
}
