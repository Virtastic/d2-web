// Shared pieces of the item-kind dispatch family D2Game 0x54420 / 0x545D0 / 0x54A20 / 0x54BC0 (1.13c, disassembled).
// Callees are reached through the D2Seam with exactly the registers/stack words retail hands them (the FN descriptor's hook REGS/NST).
#pragma once
#include "d2game_seam4e.h"
// 0xD76A0 gate: edx = a table id, eight stack words (ret 0x20).
static inline uint32_t d2ik_gate(const D2Seam* e, uint32_t edx, const uint32_t w[8])
{
    const uint32_t r = e->call(0xD76A0, 0, 0, edx, 0, 0, w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7]);
    return r;
}
// 0xD4A70: eax, ecx and eight stack words (ret 0x20).
static inline uint32_t d2ik_d4a70(const D2Seam* e, uint32_t eax, uint32_t ecx, const uint32_t w[8])
{
    const uint32_t r = e->call(0xD4A70, eax, ecx, 0, 0, 0, w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7]);
    return r;
}
// The kind-1 (equip) branch of 0x54420 and 0x54A20: false = the caller's failure (0xD), true = the equip ran (545A0 called).
static inline bool d2ik_equip(const D2Seam* e, uint32_t item, uint32_t ctx, uint32_t game)
{
    const uint32_t eng = d2rd32(ctx + 0x60);
    const uint32_t cls = e->call(0xA97C, 0, 0, 0, 0, 0, item) & 0xFF;
    if (!cls) return false;
    if (!e->call(0xA9A0, 0, 0, 0, 0, 0, eng, item, cls)) return false;
    const uint32_t mode = (cls == 0xB || cls == 0xC) ? 4 : 3;
    if (!e->call(0xA9B8, 0, 0, 0, 0, 0, eng, item, mode)) return false;
    e->call(0xA86E, 0, 0, 0, 0, 0, item, cls);
    d2wr32(item + 0xC4, d2rd32(item + 0xC4) & 0xFFFFFFFDu);
    e->call(0xA4A2, 0, 0, 0, 0, 0, item, 1);
    e->call(0xA92E, 0, 0, 0, 0, 0, item, 0xFF);
    e->call(0xA916, 0, 0, 0, 0, 0, item, 8, 1);
    d2wr32(item + 0xC4, d2rd32(item + 0xC4) & 0xFDFFFFFFu);
    e->call(0xA994, 0, 0, 0, 0, 0, eng, item);
    e->call(0xA9A6, 0, 0, 0, 0, 0, ctx, 1);
    e->call(0x345A0, item, game, ctx);
    return true;
}
// The item's unit-class field helpers 0x1100 / 0x1130 (ecx = item, plain ret).
static inline uint32_t d2ik_1100(const D2Seam* e, uint32_t item) { return e->call(0x1100, 0, item); }
static inline uint32_t d2ik_1130(const D2Seam* e, uint32_t item) { return e->call(0x1130, 0, item); }
