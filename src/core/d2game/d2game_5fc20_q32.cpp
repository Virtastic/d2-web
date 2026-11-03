// 0x5FC20 (ECX = quest record): A5Q3 init (Quests/a5q3.cpp). Clears the 0xF dwords at +0xA0, installs the handler table (module addresses, written as
// 0xB2000000|rva), allocates the 0x114-byte data block from the game pool (game = quest[1], pool at game+0x1C; Fog alloc thunk 0xA2E6, line 0x5CA),
// zeroes it (the word at +0x80 and dwords +0x84/+0x88 are then re-stored as zero).
#include "d2game_q32.h"
using namespace q32;
static uint32_t M(uint32_t rva) { return 0xB2000000u | rva; }
// 1.13c D2Game+0x5FC20
void d2game_5fc20_q32(const Call& X, uint32_t q)
{
    for (uint32_t o = 0xa0; o < 0xa0 + 0x3c; o += 4) wr32(q, o, 0);
    wr32(q, 0xa0, M(0x5da70)); wr32(q, 0xc8, M(0x58040)); wr32(q, 0xac, M(0x5f3f0)); wr32(q, 0xa8, M(0x5a0a0));
    wr32(q, 0xcc, M(0x5f5e0)); wr32(q, 0xd4, M(0x57e80)); wr32(q, 0xd8, M(0x57dd0)); wr32(q, 0xdc, M(0x10bcc0));
    wr8(q, 0xa, 1); wr8(q, 0xc, 0);
    uint32_t d = X(0xA2E6, 0, rd32(rd32(q, 4), 0x1c), 0x114, 0, 0, M(0xfc060), 0x5ca, 0);
    wr32(q, 0x18, d);
    wr32(q, 0xe0, 0x25); wr32(q, 0xe8, M(0x5b8c0)); wr8(q, 0xd, 5);
    wr32(q, 0xec, M(0x57f50)); wr32(q, 0xf0, M(0x56b10));
    wr32(q, 0x10, 0x22);
    for (uint32_t o = 0; o < 0x114; o += 4) wr32(d, o, 0);
    wr16(d, 0x80, 0); wr32(d, 0x84, 0); wr32(d, 0x88, 0);
}
