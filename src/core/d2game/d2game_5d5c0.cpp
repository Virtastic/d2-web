// Act 5 quest 4 record init: zero the callback table, install the A5Q4 handlers,
// allocate the 0x90-byte state. Callees are seams: poolAlloc is the Fog pool allocator (thunk 0xA2E6), `mod` the module base.
#include "d2game_2e790.h"
using namespace d2g32;

// 1.13c D2Game+0x5D5C0 (ECX = quest record)
void ACT5Q4_QuestInit(uint32_t q, D2GPoolAlloc poolAlloc, uint32_t file, uint32_t mod)
{
    for (uint32_t o = 0xA0; o < 0xA0 + 15 * 4; o += 4) wr32(q + o, 0);
    wr8(q + 0xC, 0);
    wr32(q + 0xA0, mod + 0x57A60);
    wr32(q + 0xC8, mod + 0x57A20);
    wr32(q + 0xAC, mod + 0x5AD50);
    wr32(q + 0xC0, mod + 0x5B690);
    wr32(q + 0xA8, mod + 0x5D520);
    wr32(q + 0xCC, mod + 0x5B7A0);
    wr32(q + 0xD4, mod + 0x5AE20);
    wr32(q + 0xDC, mod + 0x10C488);
    wr8(q + 0xA, 1);
    const uint32_t s = poolAlloc(rd32(rd32(q + 4) + 0x1C), 0x90, file, 0x30D, 0);
    wr32(q + 0x18, s);
    wr32(q + 0xE0, 0x26);
    wr32(q + 0xE8, mod + 0x568C0);
    wr8(q + 0xD, 4);
    wr32(q + 0xEC, mod + 0x56840);
    wr32(q + 0xF0, mod + 0x5AFB0);
    wr32(q + 0x10, 0x23);
    for (uint32_t o = 0; o < 0x90; o += 4) wr32(s + o, 0);
    wr8(s + 0x80, 0);
    wr8(s + 0x81, 0);
}
