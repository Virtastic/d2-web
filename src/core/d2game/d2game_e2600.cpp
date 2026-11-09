// 1.13c D2Game+0xE2600: stdcall (game, pct), ESI = unit. Re-times a unit's frame schedule.
#include "d2game_h16.h"
namespace h16 {
void e2600(const Env& e, uint32_t game, int32_t pct, uint32_t esi)
{
    if (esi) {
        for (uint32_t ty = 0; ty < 2; ++ty)
            for (uint32_t n = rd32(esi, 0xdc); n;) {
                uint32_t t = rd8(n, 0), next = rd32(n, 0x24);
                if (t == ty) e.s8e6f0(n, game);
                n = next;
            }
    }
    uint32_t frame = rd32(game, 0xa8);
    int32_t q = (int32_t)((frame - (uint32_t)((int32_t)rd32(esi, 0x44) >> 8)) * (uint32_t)(100 - pct)) / 100;
    wr32(esi, 0x44, (frame - (uint32_t)q) << 8);
    int32_t step, limit;
    if (rd32(esi, 0x30) == 0) { step = (int16_t)rd32(esi, 0x4c); limit = (int32_t)rd32(esi, 0x48); }
    else { step = (int32_t)rd32(esi, 0x3c); limit = (int32_t)rd32(esi, 0x34); }
    if (step == 0) { e.s8ea80(game, esi, 1, rd32(game, 0xa8) + 1, 0, 0, 0); return; }
    uint32_t tbl = rd32(esi, 0x50);
    int32_t lo = q - 1, pos = (int32_t)((uint32_t)q << 8), nxt = (int32_t)((uint32_t)pos + (uint32_t)step);
    while (nxt < limit) {
        int32_t a = (int32_t)((uint32_t)pos + (uint32_t)step), b = (int32_t)((uint32_t)nxt + (uint32_t)step), hi = a >> 8;
        ++frame;
        for (; lo <= hi; ++lo) {
            uint32_t v;
            if (rd32(esi, 0x30) == 0) v = rd8(tbl, (uint32_t)lo + 0x10);
            else e.o10153(rd32(esi, 0x30), (uint32_t)lo << 8, &v);
            if (0 < (int32_t)v && (int32_t)v < 5) e.s8ea80(game, esi, 0, frame, 0, v, 0);
        }
        pos = a; nxt = b;
    }
    if (frame == rd32(game, 0xa8)) ++frame;
    e.s8ea80(game, esi, 1, frame + 1, 0, 0, 0);
}
}
