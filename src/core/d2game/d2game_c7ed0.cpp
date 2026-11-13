// 1.13c D2Game+0xC7ED0 -- Act 2 quest 2 node constructor (ECX = quest node); written from the 1.13c disassembly.
// Clears the callback area (node +0xA0 .. +0xDB), then sets the node: +0xDC a data table, +0xA byte 1, +0x18 a 0x30-byte zeroed block from the game pool
// (0xA2E6: ECX game +0x1C pool, EDX 0x30, source file a2q2.cpp, line 0x3ED), +0xE0 = 10, +0xD byte 0, +0xB byte 0xD, and the callback words
// (+0xA0, AC, B0, B4, C0, C4, C8, CC, D4, D8, E8, EC). The words are module code and data addresses, stored as module-relative values here.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fc7ed0(HookEnv& e, uint32_t node)
{
    for (uint32_t o = 0xA0; o < 0xDC; o += 4) e.wr32(node + o, 0);
    e.wr32(node + 0xDC, 0xB2000000u | 0x103988);
    e.wr8(node + 0xA, 1);
    const uint32_t blk = e.h(0xA2E6, 0, e.rd32(e.rd32(node + 4) + 0x1C), 0x30, 0, 0, 0xB2000000u | 0xFB314, 0x3ED, 0);
    e.wr32(node + 0x18, blk);
    e.wr32(node + 0xE0, 0xA);
    e.wr32(node + 0xE8, 0xB2000000u | 0xC3BD0);
    e.wr8(node + 0xD, 0);
    e.wr32(node + 0xEC, 0xB2000000u | 0xC5440);
    e.wr32(node + 0xAC, 0xB2000000u | 0xC3EC0);
    e.wr32(node + 0xA0, 0xB2000000u | 0xC5310);
    e.wr32(node + 0xCC, 0xB2000000u | 0xC7DC0);
    e.wr32(node + 0xC8, 0xB2000000u | 0xC3EB0);
    e.wr32(node + 0xB0, 0xB2000000u | 0xC5F30);
    e.wr32(node + 0xB4, 0xB2000000u | 0xC3E10);
    e.wr32(node + 0xC0, 0xB2000000u | 0xC2450);
    e.wr32(node + 0xD4, 0xB2000000u | 0xC3A80);
    e.wr32(node + 0xD8, 0xB2000000u | 0xC39B0);
    e.wr32(node + 0xC4, 0xB2000000u | 0xC23C0);
    for (uint32_t o = 0; o < 0x30; o += 4) e.wr32(blk + o, 0);
    e.wr8(node + 0xB, 0xD);
    return 0;
}
}
