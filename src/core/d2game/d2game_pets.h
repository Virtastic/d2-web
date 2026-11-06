// 1.13c D2Game pet-list helpers 0x972C0, 0x97120, 0x97730, written from the disassembly as rules over E (see d2game_7e070.h). Pool file tag 0xB20FBA38 is what a module string pointer logs as.
// 972C0: EAX = &list head, ECX (carried to 971F0 as ESI), EBX = game, stack w0/w1 (ret 8): for each node (+0x14 next) whose flag byte bit 0 is clear, look its id (+4) up in the game unit table
//   (game+0x1320+(id&0x7F)*4, chain +0xE4, id at +0xC); a hit calls 971F0(EAX unit, ESI ecx, stack game w0 w1) once. A null game with a live node is the fatal path (not modelled).
// 97120: EBX = game, stack unit (ret 4): player data = A4C0(unit); an existing pets block (+0x44) goes to 97640(EAX old, ECX game, EDX unit); a 4-byte holder and a count*12 list block come from the game pool
//   (game+0x1C), the list is zeroed, then 96F40(EAX unit, stack game). count = [[sgptDataTables]+0xBF0].
// 97730: EBX = unit, stack game (ret 4): pets block (+0x44) of the player data; for type 1..count-1 97420(EAX game, stack unit, [pets]+12*type, t==7, t==7); then 96FA0(game, unit).
#pragma once
#include <cstdint>
namespace gpets {
constexpr uint32_t kFile = 0xB20FBA38u;
template <class E> void F972C0(E& e, uint32_t head, uint32_t ecx, uint32_t game, uint32_t w0, uint32_t w1)
{
    for (uint32_t n = e.rd32(head); n; n = e.rd32(n + 0x14)) {
        if (e.rd8(n) & 1) continue;
        if (!game) abort();
        const uint32_t id = e.rd32(n + 4);
        for (uint32_t u = e.rd32(game + 0x1320 + (id & 0x7F) * 4); u; u = e.rd32(u + 0xE4))
            if (e.rd32(u + 0xC) == id) { e.h(0x971F0, u, 0, 0, 0, ecx, game, w0, w1); break; }
    }
}
template <class E> void F97120(E& e, uint32_t game, uint32_t unit, uint32_t count)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    if (!pd) abort();
    const uint32_t old = e.rd32(pd + 0x44);
    if (old) e.h(0x97640, old, game, unit);
    const uint32_t pets = e.h(0xA2E6, 0, e.rd32(game + 0x1C), 4, 0, 0, kFile, 0xA2, 0);
    e.wr32(pets, 0); e.wr32(pd + 0x44, pets);
    const uint32_t lists = e.h(0xA2E6, 0, e.rd32(game + 0x1C), count * 12, 0, 0, kFile, 0xA6, 0);
    e.wr32(pets, lists);
    for (uint32_t i = 0; i < count * 12; i++) e.wr8(lists + i, 0);
    e.h(0x96F40, unit, 0, 0, 0, 0, game);
}
template <class E> void F97730(E& e, uint32_t unit, uint32_t game, uint32_t count)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    if (!pd) abort();
    const uint32_t pets = e.rd32(pd + 0x44);
    if (!pets) return;
    for (int32_t t = 1; t < (int32_t)count; t++) {
        const uint32_t ent = e.rd32(pets) + (uint32_t)t * 12;
        e.h(0x97420, game, 0, 0, 0, 0, unit, ent, t == 7, t == 7);
    }
    e.h(0x96FA0, 0, 0, 0, 0, 0, game, unit);
}
}
