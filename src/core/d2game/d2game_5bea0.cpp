// Act 5 quest 1 (Siege on Harrogath) record init: zero the callback table, install the A5Q1 handlers, allocate the 0x18-byte state.
// Callees are seams: poolAlloc is the Fog pool allocator (thunk 0xA2E6), `mod` the module base the handler RVAs are added to.
#include "d2game_2e790.h"
using namespace d2g32;

// 1.13c D2Game+0x5BEA0 (ECX = quest record)
void ACT5Q1_QuestInit(uint32_t q, D2GPoolAlloc poolAlloc, uint32_t file, uint32_t mod)
{
    for (uint32_t o = 0xA0; o < 0xA0 + 15 * 4; o += 4) wr32(q + o, 0);
    wr8(q + 0xC, 0);
    wr32(q + 0xA0, mod + 0x587C0);
    wr32(q + 0xC8, mod + 0x587A0);
    wr32(q + 0xAC, mod + 0x5B380);
    wr32(q + 0xC0, mod + 0x5BDB0);
    wr32(q + 0xA8, mod + 0x5A720);
    wr32(q + 0xCC, mod + 0x5B4F0);
    wr32(q + 0xD4, mod + 0x59450);
    wr32(q + 0xDC, mod + 0x10ACB8);
    wr8(q + 0xA, 1);
    const uint32_t s = poolAlloc(rd32(rd32(q + 4) + 0x1C), 0x18, file, 0x2D3, 0);
    wr32(q + 0x18, s);
    wr32(q + 0xE0, 0x23);
    wr32(q + 0xE8, mod + 0x56E00);
    wr8(q + 0xD, 4);
    wr32(q + 0xEC, mod + 0x56D80);
    wr32(q + 0xF0, mod + 0x56D10);
    wr32(q + 0x10, 0x20);
    for (uint32_t o = 0; o < 0x18; o += 4) wr32(s + o, 0);
}
