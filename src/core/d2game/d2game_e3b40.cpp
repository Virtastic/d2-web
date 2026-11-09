// 1.13c D2Game+0xE3B40: ECX = quest node (data at +0x18), EDX = {game, ?, ?, unit}.
// The decompile's iVar4 / uRam writes are mislabelled registers: the stores are node[9], node[0xb], node[0xc] and the data bytes +1/+0xc.
#include "d2game_h16.h"
namespace h16 {
void e3b40(const Env& e, uint32_t node, uint32_t a)
{
    uint32_t game = rd32(a, 0), unit = rd32(a, 0xc);
    uint32_t sk = rd32(e.o10920(unit), 0x10 + rd8(game, 0x6d) * 4);
    uint32_t d = rd32(node, 0x18);
    if (e.o10174(sk, 0x14, 0) != 0 || e.o10174(sk, 0x14, 1) != 0) { wr8(node, 9, 0); wr8(d, 1, 0); wr8(d, 0xc, 0); return; }
    uint32_t r1 = e.findQuest(unit, game, 0x20343367);
    uint32_t r2 = e.findQuest(unit, game, 0x2034336a);
    if (e.o10174(sk, 0x14, 1) != 0) { wr8(node, 9, 0); wr8(d, 1, 0); wr8(d, 0xc, 0); return; }
    if (r1 != 0) {
        wr8(d, 1, 0); wr8(d, 0xc, 0);
        bool four = e.o10174(sk, 0x14, 4) != 0;
        wr8(node, 0xc, 3); wr8(node, 0xb, four ? 4 : 3);
        return;
    }
    if (r2 == 0) return;
    wr8(d, 1, 0); wr8(d, 0xc, 0);
    bool two = e.o10174(sk, 0x14, 2) != 0;
    wr8(node, 0xc, two ? 2 : 1); wr8(node, 0xb, two ? 2 : 1);
}
}
