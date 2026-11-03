// 1.13c D2Game+0x24E70 (sub_6FC6FBA0, spawns a monster via 0x24950 and applies quest-spawn flags): EAX, ECX, EDX are forwarded to the spawner; stack (ret 0x20) a1 = game,
// a2, a3, a4 = word stored at data +0x14, a5 = toggle for data byte +0x16 bit 4, a6 = flag, a7 = word case code, a8 = pointer to 9 umod bytes. Returns the unit (residue for the capture: retmask 0).
// Written from the 1.13c disassembly as a rule over E. Spawner 0x24950 (EAX = 1, ECX = a3, EDX = 0; stack a1, a2, EDX, ECX, EAX of entry) returns the unit or NULL. Unit +0 type, +0x14 data.
// Only a monster (type 1) with data goes on: data +0x16 bit 4 := a5 != 0; umod bytes copied to data +0x1C..+0x24; 0x248A0 (EAX = 0, ECX = 0, EDX = a1; stack a2, 0, 0); data +0x14 := a4 (word);
// any umod byte 0x1E calls 0x24CD0 (ECX = unit, EDX = 0x1E, stack 1). a6 == 0 ends; else data +0x16 |= 2, data +0x26 := a7 (word) and the switch on the case code: 6 / 0x2A call 0xA3CA(unit, 0x76, 1) first,
// then find the quest-list entry (list game +0x10F4, next at +0xF4) of type 5 (6), 0x1F (0x2A), 0x13 (0x1A / 0x1B / 0x1D), 0x17 (0x24..0x26), 0x23 (0x2B..0x2D), 0x22 (0x3C) and call 0x33F80(game);
// 0x27 calls 0x35890 (EAX = unit, EBX = 4, stack game).
#pragma once
#include <cstdint>

namespace g24e70 {
template <class E> void Spawn(E& e, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8)
{
    // 1.13c D2Game+0x24E70
    const uint32_t u = e.h(0x24950, 1, a3, 0, 0, 0, a1, a2, edx, ecx, eax);
    if (!u || e.rd32(u) != 1) return;
    uint32_t d = e.rd32(u + 0x14);
    if (!d) return;
    if (a5) e.wr8(d + 0x16, e.rd8(d + 0x16) | 4); else e.wr8(d + 0x16, e.rd8(d + 0x16) & 0xFB);
    bool has = false;
    for (uint32_t i = 0; i < 9; i++) { const uint32_t b = e.rd8(a8 + i); e.wr8(d + 0x1C + i, b); if (b == 0x1E) has = true; }
    e.h(0x248A0, 0, 0, a1, 0, 0, a2, 0, 0);
    if (e.rd32(u) == 1 && e.rd32(u + 0x14)) { d = e.rd32(u + 0x14); e.wr8(d + 0x14, a4); e.wr8(d + 0x15, a4 >> 8); }
    if (has) e.h(0x24CD0, 0, u, 0x1E, 0, 0, 1);
    if (!a6) return;
    if (e.rd32(u) == 1) {
        d = e.rd32(u + 0x14);
        if (d) e.wr8(d + 0x16, e.rd8(d + 0x16) | 2);
        if (e.rd32(u) == 1 && (d = e.rd32(u + 0x14))) { e.wr8(d + 0x26, a7); e.wr8(d + 0x27, a7 >> 8); }
    }
    const int32_t k = (int32_t)(int16_t)a7;
    uint32_t type = 0;
    switch (k) {
    case 6: e.h(0xA3CA, 0, 0, 0, 0, 0, u, 0x76, 1); type = 5; break;
    case 0x1A: case 0x1B: case 0x1D: type = 0x13; break;
    case 0x24: case 0x25: case 0x26: type = 0x17; break;
    case 0x27: e.h(0x35890, u, 0, 0, 4, 0, a1); return;
    case 0x2A: e.h(0xA3CA, 0, 0, 0, 0, 0, u, 0x76, 1); type = 0x1F; break;
    case 0x2B: case 0x2C: case 0x2D: type = 0x23; break;
    case 0x3C: type = 0x22; break;
    default: return;
    }
    const uint32_t list = e.rd32(a1 + 0x10F4);
    if (!list) return;
    uint32_t n = e.rd32(list);
    while (n) { if (e.rd32(n) == type) { e.h(0x33F80, 0, 0, 0, 0, 0, a1); return; } n = e.rd32(n + 0xF4); }
}
}
