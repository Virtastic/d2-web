// 1.13c D2Game+0xA4E40 -- start skill 0x11E on a unit and arm the AI step. ESI = unit, EDI = ctx (a pointer to the AI
// record pointer), stack arg0 (ret 4). Calls, in order: thunk 0xA9FA(unit, 0x11E, 1, 0, file, 0x1700) (stdcall, 6 words);
// skill = thunk 0xA3F4 (ECX = unit, EDX = 0x11E), null is retail's fatal assert; thunk 0xAC94(skill, arg0) (stdcall, 2).
// A null unit is a fatal assert. owner = unit+0x80. For unit types 2, 4 and 5, d = unit+0x2C gives x = d+0x10 and
// y = d+0xC; otherwise x = d ? thunk 0xA3B8(d) : 0 and y = d ? thunk 0xA454(d) : 0 (stdcall, 1 word each). Then
// D2Game+0xF2380 (AL = 0xA over y's upper bytes, ECX = unit; 0x11E, 0, y, x + 0xD; ret 0x10). Finally the record
// *ctx: +0x14 += 1, +0x1C = owner+0xA8 + 100, +0x18 |= 2 then &= ~1. EAX is left holding *ctx.
#pragma once
#include <cstdint>
#include <cstdlib>
namespace ga4e40 {
// 1.13c D2Game+0xA4E40
template <class E>
uint32_t Start(E& e, uint32_t unit, uint32_t ctx, uint32_t arg0)
{
    e.h(0xA9FA, 0, 0, 0, 0, 0, unit, 0x11E, 1, 0, 0x6FD1B6B0u, 0x1700);
    const uint32_t skill = e.h(0xA3F4, 0, unit, 0x11E);
    if (!skill) std::abort();
    e.h(0xAC94, 0, 0, 0, 0, 0, skill, arg0);
    if (!unit) std::abort();
    const uint32_t owner = E::rd32(unit + 0x80);
    const uint32_t t = E::rd32(unit);
    const bool direct = t == 2 || t == 4 || t == 5;
    const uint32_t d = E::rd32(unit + 0x2C);
    const uint32_t x = direct ? E::rd32(d + 0x10) : d ? e.h(0xA3B8, 0, 0, 0, 0, 0, d) : 0;
    const uint32_t y = direct ? E::rd32(d + 0xC) : d ? e.h(0xA454, 0, 0, 0, 0, 0, d) : 0;
    e.h(0xF2380, (y & 0xFFFFFF00u) | 0xA, unit, 0, 0, 0, 0x11E, 0, y, x + 0xD);
    E::wr32(E::rd32(ctx) + 0x14, E::rd32(E::rd32(ctx) + 0x14) + 1);
    E::wr32(E::rd32(ctx) + 0x1C, E::rd32(owner + 0xA8) + 100);
    E::wr32(E::rd32(ctx) + 0x18, E::rd32(E::rd32(ctx) + 0x18) | 2);
    E::wr32(E::rd32(ctx) + 0x18, E::rd32(E::rd32(ctx) + 0x18) & ~1u);
    return E::rd32(ctx);
}
}
