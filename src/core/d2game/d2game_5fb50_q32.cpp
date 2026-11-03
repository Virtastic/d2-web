// 0x5FB50 (ECX = quest record): A5Q5 init (Quests/a5q5.cpp). Clears the 0xF dwords at +0xA0, installs the handler table (module addresses, written as
// 0xB2000000|rva), allocates the 0x64-byte data block from the game pool (game = quest[1], pool at game+0x1C; Fog alloc thunk 0xA2E6, line 0x41D)
// and zeroes it.
#include "d2game_q32.h"
using namespace q32;
static uint32_t M(uint32_t rva) { return 0xB2000000u | rva; }
// 1.13c D2Game+0x5FB50
void d2game_5fb50_q32(const Call& X, uint32_t q)
{
    for (uint32_t o = 0xa0; o < 0xa0 + 0x3c; o += 4) wr32(q, o, 0);
    wr8(q, 0xc, 0);
    wr32(q, 0xa0, M(0x575f0)); wr32(q, 0xc8, M(0x575d0)); wr32(q, 0xac, M(0x5ab60)); wr32(q, 0xc0, M(0x5eed0));
    wr32(q, 0xa8, M(0x59be0)); wr32(q, 0xcc, M(0x5f200)); wr32(q, 0xd4, M(0x57540)); wr32(q, 0xdc, M(0x10cac0));
    wr8(q, 0xa, 1);
    uint32_t d = X(0xA2E6, 0, rd32(rd32(q, 4), 0x1c), 0x64, 0, 0, M(0xfc018), 0x41d, 0);
    wr32(q, 0x18, d);
    wr32(q, 0xe0, 0x27); wr32(q, 0xe8, 0); wr8(q, 0xd, 4);
    wr32(q, 0xec, M(0x56810)); wr32(q, 0xf0, M(0x567a0));
    wr32(q, 0x10, 0x24);
    for (uint32_t o = 0; o < 0x64; o += 4) wr32(d, o, 0);
}
