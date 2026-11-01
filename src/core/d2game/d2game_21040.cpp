// Decompile-derived, 1.13c-only. No callees on the played path (the null-game branch is Fog fatal-exit and is not vectored).
#include <cstdint>
#include <cstring>

static uint32_t r32(uint32_t base, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(base + o), 4); return v; }
static void w32(uint32_t base, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(base + o), &v, 4); }

// 1.13c D2Game+0x21040
// EAX = party/group block: game at +0x28, member list at +0x34 (entry: id +0, next +4). For each entry finds the unit with that id in the game's
// hash table (game+0x1320 + (id & 0x7F) * 4, chain next at unit+0xE4, id at unit+0xC); a player unit (type 1) has its AI block's +0x2C := -1,
// +0x30 := 1, +0x28 := game (block = pData(+0x14)->+0x28).
extern "C" void d2game_attach_party_game_21040(uint32_t party)
{
    const uint32_t game = r32(party, 0x28);
    for (uint32_t e = r32(party, 0x34); e; e = r32(e, 4)) {
        const uint32_t id = r32(e, 0);
        for (uint32_t u = r32(game, 0x1320 + (id & 0x7f) * 4); u; u = r32(u, 0xe4)) {
            if (r32(u, 0xc) != id) continue;
            if (r32(u, 0) == 1) {
                const uint32_t pd = r32(u, 0x14);
                const uint32_t blk = pd ? r32(pd, 0x28) : 0;
                w32(blk, 0x2c, 0xffffffffu); w32(blk, 0x30, 1); w32(blk, 0x28, game);
            }
            break;
        }
    }
}
