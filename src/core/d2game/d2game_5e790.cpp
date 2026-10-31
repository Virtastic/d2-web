// src/core/d2game/d2game_5e790.cpp -- 1.13c D2Game+0x5E790, written from the 1.13c disassembly. Retail
// 32-bit layout (dword pointers); seams are function pointers in retail argument order.
#include <cstdint>
#include <cstring>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdp (const void* p, uint32_t o) { return (void*)(uintptr_t)rd32(p, o); }

// 1.13c D2Game+0x5E790: ESI = game, EDI = unit, plain ret. Find quest
// 0x22 in the game's quest list (game+0x10F4 -> head; id at +0, next at +0xF4); when its record (+0x18)
// has the active byte +0x87 set and 0x5D380(EAX = unit, quest) says no (al == 0), call
// 0x8EA80(ECX = game, EDX = unit, 7, game+0xA8 dword + 0xC, 0, 0, 0). 1.10f has no twin.
extern "C" void d2game_5e790_probe(void* game, void* unit,
    uint8_t (*questFlag5d380)(void* unit, void* quest),
    void (*fn8ea80)(void* game, void* unit, int32_t a, uint32_t b, int32_t c, int32_t d, int32_t e))
{
    const void* list = rdp(game, 0x10F4);
    if (!list) return;
    void* q = rdp(list, 0);
    while (q && rd32(q, 0) != 0x22) q = rdp(q, 0xF4);
    if (!q) return;
    if (rd8(rdp(q, 0x18), 0x87) == 0) return;
    if (questFlag5d380(unit, q) != 0) return;
    fn8ea80(game, unit, 7, rd32(game, 0xA8) + 0xC, 0, 0, 0);
}
