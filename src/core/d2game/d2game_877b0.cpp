// 1.13c D2Game+0x877B0 -- an AI think handler (ECX = game, EDX = unit, stack = tick block, ret 4). Written from the 1.13c disassembly as a
// rule over E (E::h = a hooked callee, E::blk = the stack block a callee is handed, E::rd32/rd16s/rd8/wr32 = memory).
// tick block tp: +0 control c (+0x14 next-frame stamp, +0x18 shots), +0x14 a scalar count, +0x1C the monstats row. Row words (index = game byte +0x6D):
// +0x56 shots, +0x5C gap. A zero stamp loads stamp = row+0x5C + game+0xA8, shots = row+0x56. Unit kind (unit+0x10) 1 or 10: kind 1 with count > 5 idles 5
// (0xF22C0, EAX = game, ESI = unit); else 0xA496(unit, 0), the 8-word packet {10 or 11, unit, 0, 0, 0, flag << 8, 0, 0} goes to 0x75E10 (game, &pkt, 1) with
// flag 0x65 when the unit's class record (0x1320, EAX = unit+4) byte +0x105 has a bit of the D2Game mask table word (+8 kind 1, +0xC kind 10), else 0x64;
// then 0xF20C0(game, unit, 0x14). Kind 11 with shots left: if stamp < game+0xA8 it re-arms the stamp, fetches the unit's position through the
// 7-word 0xAC64 (four out words), 0xA478, 0x1280(EAX = dir, EBX = y, EDI = z) and, if that yields a record, fires 0xEF650; a hit takes one shot, ORs 0x4020000 into
// its +0xC4 and, for unit class 0x2C7, sets stat 0xB8 = 1 and state 0xCA = 0. Ends with 0xF20C0(game, unit, row+0x5C). Anything else: 0xA496, packet
// {0, unit, 0, 0, 0, 0x6400, 0, 0}, 0x75E10.
#ifndef D2GAME_RULE_877B0
#define D2GAME_RULE_877B0
#include <cstdint>

namespace g877b0 {
template <class E> void Think(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t ga)
{
    const uint32_t cnt = e.rd32(tp + 0x14), tbl = e.rd32(tp + 0x1C), c = e.rd32(tp);
    const uint32_t kind = unit ? e.rd32(unit + 0x10) : 0, d = e.rd8(game + 0x6D) * 2;
    if ((int32_t)e.rd32(c + 0x14) < 1) {
        e.wr32(c + 0x14, (uint32_t)e.rd16s(tbl + 0x5C + d) + e.rd32(game + 0xA8));
        e.wr32(c + 0x18, (uint32_t)e.rd16s(tbl + 0x56 + d));
    }
    if (kind == 1 || kind == 10) {
        if (kind == 1 && (int32_t)cnt > 5) { e.h(0xF22C0, game, 0, 0, 0, unit, 5); return; }
        e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
        uint32_t flag = 0x64;
        const uint32_t cr = e.h(0x1320, e.rd32(unit + 4));
        if (cr && (e.rd8(cr + 0x105) & e.rd32(ga + (kind == 1 ? 8 : 0xC)))) flag = 0x65;
        uint32_t pkt[8] = { kind == 1 ? 10u : 11u, unit, 0, 0, 0, flag << 8, 0, 0 };
        e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(pkt), 1);
        e.blk(0x75E10, pkt, 8);
        e.h(0xF20C0, game, 0, 0, unit, 0, 0x14);
        return;
    }
    if (kind == 11 && (int32_t)e.rd32(c + 0x18) > 0) {
        const uint32_t frame = e.rd32(game + 0xA8);
        if ((int32_t)e.rd32(c + 0x14) < (int32_t)frame) {
            e.wr32(c + 0x14, (uint32_t)e.rd16s(tbl + 0x5C + d) + frame);
            uint32_t x = 0, y = 0, z = 0, w = 0;
            e.h(0xAC64, 0, 0, 0, 0, 0, unit, e.stk(&x), e.stk(&y), e.stk(&z), e.stk(&w), 0, 0);
            const uint32_t dir = e.h(0xA478, 0, 0, 0, 0, 0, unit);   // the result stays in EAX for 0x1280
            const uint32_t sk = e.h(0x1280, dir, 0, 0, y);
            e.edi(0x1280, z);
            if (sk) {
                const uint32_t m = e.h(0xEF650, 0, x, w, 0, 0, game, sk, y, z, 2, 0x42);
                if (m) {
                    e.wr32(c + 0x18, e.rd32(c + 0x18) - 1);
                    e.wr32(m + 0xC4, e.rd32(m + 0xC4) | 0x4020000u);
                    if (e.rd32(unit + 4) == 0x2C7) { e.h(0xA3CA, 0, 0, 0, 0, 0, m, 0xB8, 1); e.h(0xA83E, 0, 0, 0, 0, 0, m, 0xCA, 0); }
                }
            }
        }
        e.h(0xF20C0, game, 0, 0, unit, 0, (uint32_t)e.rd16s(tbl + 0x5C + d));
        return;
    }
    e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
    uint32_t pkt[8] = { 0, unit, 0, 0, 0, 0x6400u, 0, 0 };
    e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(pkt), 1);
    e.blk(0x75E10, pkt, 8);
}
}
#endif
