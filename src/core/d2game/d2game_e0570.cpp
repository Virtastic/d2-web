// 1.13c D2Game+0xE0570, from the disassembly. Each unit that is a player (type 1) with a unit+0x14, +0x28 chain to {+0x28 game,
// +0x2C guid, +0x30 type} is replaced by the unit E03A0 finds for that triple (kept when E03A0 returns 0). Equal pointers: 1. If either
// is not a type-0 unit: ordinal 10666(A, B) (A = ECX, B = EAX). Otherwise equal 9BEB0 words (0xFFFF when unit+0x80 is 0) that are not
// 0xFFFF give 1, else 0.
#include "d2game_e0570.h"
#include <cstring>
namespace {
uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
uint32_t resolve(uint32_t u, const D2GameE0570Env* e) {
    if (!u || rd(u, 0) != 1) return u;
    const uint32_t a = rd(u, 0x14);
    if (!a) return u;
    const uint32_t b = rd(a, 0x28);
    if (!b) return u;
    const uint32_t game = rd(b, 0x28);
    if (!game) return u;
    const uint32_t f = e->findUnit(game, rd(b, 0x30), rd(b, 0x2c));
    return f ? f : u;
}
}
uint32_t d2game_e0570_port(uint32_t eax, uint32_t ecx, const D2GameE0570Env* e) {
    uint32_t esi = resolve(ecx, e);
    uint32_t ebx = resolve(eax, e);
    if (esi == ebx) return 1;
    if (!esi || rd(esi, 0) != 0 || !ebx || rd(ebx, 0) != 0) return e->ordinal10666(esi, ebx);
    const uint32_t a = rd(esi, 0x80) ? (e->sub9BEB0(esi) & 0xffff) : 0xffff;
    const uint32_t b = rd(ebx, 0x80) ? (e->sub9BEB0(ebx) & 0xffff) : 0xffff;
    return (a == b && b != 0xffff) ? 1 : 0;
}
