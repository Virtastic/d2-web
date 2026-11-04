// D2Game+0xD4A70 -- socket filler placed into an item. EAX = filler guid; stack a0 game, a1 owner unit (or null), a2 target guid, a3 *placed,
// a4, a5, a6, a7 (ret 0x20). Written from the 1.13c disassembly; callees are the D2Seam (null-game assert and the 0xA9B8 failure assert exit: not modelled).
// *a3 = 0; a1 != 0: 0xD29F0(a1). The game's unit hash (game + 0x1720 + (guid & 0x7F) * 4, next at +0xE4, guid at +0xC) finds the filler (miss: *a3 = 1,
// 0) and the target; a filler whose type is not 4 returns 0, whose mode ([+0x10]) is not 4 sets *a3 = 1. A target of another type returns 0.
// Gates (each failing returns 0): 0xA8A4(tg, 0x10, 0x13DC), a6 != 0 with the target's mode not 0 / 1, both types 4, 0xA89E(fl), 0xA8A4(fl, 0x10, 0x13EB),
// 0xA8A4(tg, 0x800, 0x13EC), 0xA9EE(tg), inv = [tg + 0x60] (else 0xA448(0, tg)), 0xA490(inv) < (u8) 0xAA84(tg). Then 0xA9B8(inv, fl, 1); [fl + 0xC4] &= ~2;
// a5 and a1: 0xAA54([a1 + 0x60], 0); 0xD47C0 (ESI = EBX = tg, EDI = fl, stack 0); 0xA99A(fl, tg); 0xA4A2(fl, 6); 0xA3C4(tg) = r. r != 0 and ([r + 0x81] == 0 or
// [game + 0x74] != 0): 0xA91C(tg, fl, 0), 0xEA70(game), 0xEA10(game), a7: 0xD2380 (EAX = game, ECX = a1, stack tg). r == 0 or the byte set with [game + 0x74] == 0:
// a4 == 0 returns 1. Then 0xA65E(tg, 1, 1); 0xA8A4(tg, 0x4000, 0x143B) set: 0xA65E(tg, 0x4000, 0); a1 != 0: 0xA9A6(a1, 1), 0xA994([a1 + 0x60], tg). Returns 1.
#include "d2game_seam4e.h"
static const uint32_t kFile = 0xB20FB14Cu;   // the source-file string at D2Game+0xFB14C, recorded as a module address
// 1.13c D2Game+0xD4A70
extern "C" uint32_t d2game_d4a70(uint32_t fg, uint32_t game, uint32_t a1, uint32_t tgGuid, uint32_t placed, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, const D2Seam* e)
{
    d2wr32(placed, 0);
    if (a1) e->call(0xD29F0, 0, 0, 0, 0, 0, a1);
    uint32_t fl = d2rd32(game + 0x1720 + (fg & 0x7F) * 4);
    while (fl && d2rd32(fl + 0xC) != fg) fl = d2rd32(fl + 0xE4);
    if (!fl) { d2wr32(placed, 1); return 0; }
    if (d2rd32(fl) != 4) return 0;
    if (d2rd32(fl + 0x10) != 4) { d2wr32(placed, 1); return 0; }
    uint32_t tg = d2rd32(game + 0x1720 + (tgGuid & 0x7F) * 4);
    while (tg && d2rd32(tg + 0xC) != tgGuid) tg = d2rd32(tg + 0xE4);
    if (!tg) { d2wr32(placed, 1); return 0; }
    if (d2rd32(tg) != 4) return 0;
    if (!e->call(0xA8A4, 0, 0, 0, 0, 0, tg, 0x10, 0x13DC, kFile)) return 0;
    if (a6) { const uint32_t m = d2rd32(tg + 0x10); if (m && m != 1) return 0; }
    if (d2rd32(fl) != 4 || d2rd32(tg) != 4) return 0;
    if (!e->call(0xA89E, 0, 0, 0, 0, 0, fl)) return 0;
    if (!e->call(0xA8A4, 0, 0, 0, 0, 0, fl, 0x10, 0x13EB, kFile)) return 0;
    if (!e->call(0xA8A4, 0, 0, 0, 0, 0, tg, 0x800, 0x13EC, kFile)) return 0;
    if (!e->call(0xA9EE, 0, 0, 0, 0, 0, tg)) return 0;
    uint32_t inv = d2rd32(tg + 0x60);
    if (!inv) { inv = e->call(0xA448, 0, 0, 0, 0, 0, 0, tg); if (!inv) return 0; }
    const uint32_t need = e->call(0xAA84, 0, 0, 0, 0, 0, tg) & 0xFF;
    if (e->call(0xA490, 0, 0, 0, 0, 0, inv) >= need) return 0;
    e->call(0xA9B8, 0, 0, 0, 0, 0, inv, fl, 1);
    d2wr32(fl + 0xC4, d2rd32(fl + 0xC4) & 0xFFFFFFFDu);
    if (a5 && a1) e->call(0xAA54, 0, 0, 0, 0, 0, d2rd32(a1 + 0x60), 0);
    e->call(0xD47C0, 0, 0, 0, tg, tg, 0);   // EBX = ESI = tg, stack word 0 (EDI = fl is not logged)
    e->call(0xA99A, 0, 0, 0, 0, 0, fl, tg);
    e->call(0xA4A2, 0, 0, 0, 0, 0, fl, 6);
    const uint32_t r = e->call(0xA3C4, 0, 0, 0, 0, 0, tg);
    const uint32_t byte = r ? d2rd8(r + 0x81) : 0;
    if (r && !(byte && !d2rd32(game + 0x74))) {
        e->call(0xA91C, 0, 0, 0, 0, 0, tg, fl, 0);
        e->call(0xEA70, 0, 0, 0, 0, 0, game);
        e->call(0xEA10, 0, 0, 0, 0, 0, game);
        if (a7) e->call(0xD2380, game, a1, 0, 0, 0, tg);
    } else if (!a4) return 1;
    e->call(0xA65E, 0, 0, 0, 0, 0, tg, 1, 1);
    if (e->call(0xA8A4, 0, 0, 0, 0, 0, tg, 0x4000, 0x143B, kFile)) e->call(0xA65E, 0, 0, 0, 0, 0, tg, 0x4000, 0);
    if (a1) { e->call(0xA9A6, 0, 0, 0, 0, 0, a1, 1); e->call(0xA994, 0, 0, 0, 0, 0, d2rd32(a1 + 0x60), tg); }
    return 1;
}
