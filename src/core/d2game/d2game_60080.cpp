// 0x60080 (ECX = quest record): A5Q6 init. Clears the 0xF dwords at +0xA0, installs the handler table (module addresses, written as 0xB2000000|rva),
// allocates the 0xA8-byte data block from the game pool (game = quest[1], pool at game+0x1C; Fog alloc thunk 0xA2E6) and seeds it.
#include "d2game_q32.h"
using namespace q32;
static uint32_t M(uint32_t rva) { return 0xB2000000u | rva; }
// 1.13c D2Game+0x60080
void d2game_60080_q32(const Call& X, uint32_t q)
{
    for (uint32_t o = 0xa0; o < 0xa0 + 0x3c; o += 4) wr32(q, o, 0);
    wr8(q, 0xc, 0);
    wr32(q, 0xa0, M(0x57000)); wr32(q, 0xc8, M(0x56fc0)); wr32(q, 0xac, M(0x5a9b0)); wr32(q, 0xc0, M(0x5fe30));
    wr32(q, 0xa8, M(0x5d920)); wr32(q, 0xcc, M(0x57270)); wr32(q, 0xd4, M(0x58930)); wr32(q, 0xd8, M(0x56eb0)); wr32(q, 0xdc, M(0x10d1c0));
    wr8(q, 0xa, 1);
    uint32_t d = X(0xA2E6, 0, rd32(rd32(q, 4), 0x1c), 0xa8, 0, 0, M(0xfbff4), 0x329, 0);
    wr32(q, 0x18, d);
    wr32(q, 0xe0, 0x28); wr32(q, 0xe8, 0); wr8(q, 0xd, 4);
    wr32(q, 0xec, M(0x56ef0)); wr32(q, 0xf0, M(0x59930));
    wr32(q, 0x10, 0x25);
    for (uint32_t o = 0; o < 0xa8; o += 4) wr32(d, o, 0);
    wr16(d, 0x80, 0);
    wr32(d, 0x94, 1); wr32(d, 0x9c, 1);
}
