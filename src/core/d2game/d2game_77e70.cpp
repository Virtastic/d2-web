// 1.13c D2Game+0x77E70 -- does the player record belong to the unit's owner (or to a party member of it)?
// EAX = unit, ECX = player record (+0 must be 0, +0xC = id), EDX = game (unit hash at +0x1120, 128 buckets, next at +0xE4, id at +0xC).
// Fatal (Fog assert, never returns): record null 0x38C, unit null 0x38D, record type nonzero 0x38E, game null 0x2D3.
// owner id = 0xAF34(unit+0x60). Equal to record+0xC gives 1. Not in the hash gives 0. Found: 0xF02A0 (EAX = id, ECX = id & 0x7F,
// ESI = record, EDI = the found unit, stack 1) decides. No 1.10f counterpart.
#include <cstdint>
#include <cstring>

struct D2Game77E70Env {
    uint32_t (*ownerId)(uint32_t v);                                  // 0xAF34 (stdcall 1)
    uint32_t (*relation)(uint32_t id, uint32_t rec, uint32_t found);  // 0xF02A0
    void (*fatal)(uint32_t code);
};

static inline uint32_t r32(uint32_t a, uint32_t o)
{
    uint32_t v;
    std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a) + o), 4);
    return v;
}

// 1.13c D2Game+0x77E70
uint32_t D2Game_77E70(uint32_t unit, uint32_t rec, uint32_t game, const D2Game77E70Env& e)
{
    if (rec == 0) { e.fatal(0x38C); return 0; }
    if (unit == 0) { e.fatal(0x38D); return 0; }
    if (r32(rec, 0) != 0) { e.fatal(0x38E); return 0; }
    const uint32_t id = e.ownerId(r32(unit, 0x60));
    if (r32(rec, 0xC) == id) return 1;
    if (game == 0) { e.fatal(0x2D3); return 0; }
    for (uint32_t n = r32(game, 0x1120 + (id & 0x7F) * 4); n != 0; n = r32(n, 0xE4))
        if (r32(n, 0xC) == id) return e.relation(id, rec, n);
    return 0;
}
