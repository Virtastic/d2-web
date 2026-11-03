// Act 5 quest 2 (Rescue on Mount Arreat) record init: zero the callback table, install the A5Q2 handlers, allocate the 0x160-byte state.
// Callees are seams: poolAlloc is the Fog pool allocator (thunk 0xA2E6), `mod` the module base the handler RVAs are added to.
#include "d2game_2e790.h"
using namespace d2g32;

// 1.13c D2Game+0x5CC70 (ECX = quest record)
void ACT5Q2_QuestInit(uint32_t q, D2GPoolAlloc poolAlloc, uint32_t file, uint32_t mod)
{
    for (uint32_t o = 0xA0; o < 0xA0 + 15 * 4; o += 4) wr32(q + o, 0);
    wr8(q + 0xC, 0);
    wr32(q + 0xA0, mod + 0x583C0);
    wr32(q + 0xC8, mod + 0x582A0);
    wr32(q + 0xAC, mod + 0x5B1E0);
    wr32(q + 0xC0, mod + 0x5A310);
    wr32(q + 0xA8, mod + 0x5A530);
    wr32(q + 0xCC, mod + 0x5C9E0);
    wr32(q + 0xD4, mod + 0x581A0);
    wr32(q + 0xD8, mod + 0x58240);
    wr32(q + 0xDC, mod + 0x10B2F0);
    wr8(q + 0xA, 1);
    const uint32_t s = poolAlloc(rd32(rd32(q + 4) + 0x1C), 0x160, file, 0x323, 0);
    wr32(q + 0x18, s);
    wr32(q + 0xE0, 0x24);
    wr32(q + 0xE8, 0);
    wr8(q + 0xD, 4);
    wr32(q + 0xEC, mod + 0x56C40);
    wr32(q + 0xF0, mod + 0x56BD0);
    wr32(q + 0x10, 0x21);
    for (uint32_t o = 0; o < 0x160; o += 4) wr32(s + o, 0);
    wr8(s + 0x80, 0);
    wr8(s + 0x81, 0);
}
