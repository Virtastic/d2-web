// 1.13c D2Game+0xE6660: ECX = quest record; the a3q0 quest init (callbacks, 0x1c-byte data block).
#include "d2game_h16.h"
namespace h16 {
void e6660(const Env& e, uint32_t q)
{
    for (uint32_t i = 0; i < 15; ++i) wr32(q, 0xa0 + 4 * i, 0);
    wr8(q, 0xc, 0); wr8(q, 0xb, 0);
    uint32_t game = rd32(q, 4);
    wr32(q, 0xa0, e.M_54C0); wr32(q, 0xcc, e.M_5540); wr32(q, 0xd4, e.M_5450); wr32(q, 0xdc, e.M_FE38);
    wr8(q, 0xa, 1);
    uint32_t d = e.alloc(rd32(game, 0x1c), 0x1c, e.M_file, 0x94, 0);
    wr32(q, 0x18, d);
    for (uint32_t i = 0; i < 7; ++i) wr32(d, 4 * i, 0);
    wr32(q, 0xe0, 0x10); wr32(q, 0xe8, e.M_3030); wr32(q, 0xec, e.M_3000);
}
}
