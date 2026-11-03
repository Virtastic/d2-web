// 0x63270 (EBX = game, stack: list, quest, flag; ret 0xC): for each unit id in list (word count at +0x80), find the unit in game's hash at
// game+0x1120+(id&0x7F)*4 (chain +0xE4, id +0xC); unless #10174 (rec, quest, 0) == 1: #10003 (rec, quest, 0xD), (rec, quest, 0); a nonzero flag stores
// its low word at unit+0x6E, unit at +0x70, calls #10158 (unit) and sets bit 0x400 of +0xC4. A null game with a nonempty list is the fatal path.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x63270
void d2game_63270_q32(const Call& X, uint32_t game, uint32_t list, uint32_t quest, uint32_t flag) {
    for (uint32_t n = 0; n < rd16(list, 0x80); n++) {
        uint32_t id = rd32(list, n * 4);
        if (game == 0) { X(0xFFFF0000u); return; }   // retail: #10265(0x2D3), #10024, _exit(-1)
        for (uint32_t u = rd32(game, 0x1120 + (id & 0x7f) * 4); u; u = rd32(u, 0xe4)) {
            if (rd32(u, 0xc) != id) continue;
            uint32_t sk = rd32(X(kO10920, 0, 0, 0, 0, 0, u), 0x10 + rd8(game, 0x6d) * 4);
            if (qGet(X, sk, quest, 0) != 1) {
                qSet(X, sk, quest, 0xd);
                qSet(X, sk, quest, 0);
                if (flag != 0) {
                    wr16(u, 0x6e, (uint16_t)flag);
                    wr32(u, 0x70, u);
                    X(0xA4DE, 0, 0, 0, 0, 0, u);
                    wr32(u, 0xc4, rd32(u, 0xc4) | 0x400);
                }
            }
            break;
        }
    }
}
