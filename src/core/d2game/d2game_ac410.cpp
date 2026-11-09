// 1.13c D2Game+0xAC410: ECX = game, EDX = unit, stack ctx (ret 4). The two-weapon-skill monster think (AiThink4.cpp):
// ctx+0 = AI state (its +0x18 recharge counter, +0x1C level, +0x14 next-think tick), ctx+8 = the candidate target,
// ctx+0x14 / ctx+0x18 = range and cast flags, ctx+0x1C = the monster's AI parameter row (per-difficulty shorts at
// +0x56/+0x5C/+0x62/+0x68, the recharge cap at +0x84). Picks one of the target's two skills, retries with the other
// on failure, falls back to the default skill, then casts it and schedules the next think.
// The skill-id getter's file-name word (module string) is masked in the capture and passed here as 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_ac410(uint32_t game, uint32_t unit, uint32_t ctx, const D2Seam* e)
{
    // 1.13c D2Game+0xAC410
    const uint32_t row = d2rd32(ctx + 0x1C);
    uint32_t ctx2 = d2rd32(ctx + 8);
    const int32_t ctx6 = (int32_t)d2rd32(ctx + 0x18);
    const int32_t ctx5 = (int32_t)d2rd32(ctx + 0x14);
    uint32_t target = 0;
    if (unit) {
        for (uint32_t n = d2rd32(unit + 0xDC); n; ) {
            const uint32_t next = d2rd32(n + 0x24);
            if (d2rd8(n) == 2) { e->call(0x8E6F0, n); e->edi(0x8E6F0, game); }
            n = next;
        }
        if (d2rd32(unit) == 1 && d2rd32(unit + 0x14)) {
            const uint32_t s = d2rd32(d2rd32(unit + 0x14) + 0x28);
            if (s && d2rd32(s + 0x28))
                target = e->call(0xE03A0, d2rd32(s + 0x30), d2rd32(s + 0x28), d2rd32(s + 0x2C));
        }
    }
    if (!target) { e->call(0xF22C0, game, 0, 0, 0, unit, 100); return 0; }
    int32_t c = (int16_t)d2rd16(row + 0x84);
    c = c <= 1 ? 1 : (c < 0x100 ? c : 0x100);
    const uint32_t diff = d2rd8(game + 0x6D);
    const uint32_t st = d2rd32(ctx);
    const int32_t rc = (int32_t)d2rd32(st + 0x18) + (-1 - (int32_t)(int16_t)d2rd16(row + 0x68 + diff * 2));
    d2wr32(st + 0x18, (uint32_t)rc);
    if (rc < 0 || rc > (c << 6)) d2wr32(st + 0x18, 0);
    const int32_t dist = (int32_t)e->call(0xAB86, 0, 0, 0, 0, 0, unit, target);
    if (ctx5 > (int16_t)d2rd16(row + 0x56 + diff * 2) || dist > (int16_t)d2rd16(row + 0x5C + diff * 2)) ctx2 = 0;
    if (e->call(0x1E140, 0, 0, 0, 0, target, game, unit, ctx2, (uint32_t)ctx6, 0, 6)) return 0;
    if (!ctx2) { e->call(0xF22C0, game, 0, 0, 0, unit, 0x19); return 0; }
    const uint32_t a = e->call(0xA3B2, 0, 0, 0, 0, 0, target);
    const uint32_t b = e->call(0xA3AC, 0, 0, 0, 0, 0, target);
    if (!a || !b) { e->call(0xF22C0, game, 0, 0, 0, unit, 0x19); return 0; }
    const uint32_t ra = e->call(0xA49C, 0, 0, 0, 0, 0, a, 0, 0xB53);
    const uint32_t rb = e->call(0xA49C, 0, 0, 0, 0, 0, b, 0, 0xB54);
    const uint32_t la = e->call(0xA3AE0, target, a, 0, 0, 0, ctx);
    const uint32_t lb = e->call(0xA3AE0, target, b, 0, 0, 0, ctx);
    e->call(0xA9FA, 0, 0, 0, 0, 0, unit, ra, la, 0);
    e->call(0xA9FA, 0, 0, 0, 0, 0, unit, rb, lb, 0);
    const uint32_t sa = e->call(0xA3D0, 0, unit, ra, 0, 0, 0xFFFFFFFFu);
    const uint32_t sb = e->call(0xA3D0, 0, unit, rb, 0, 0, 0xFFFFFFFFu);
    if (!sa || !sb) { e->call(0xF22C0, game, 0, 0, 0, unit, 0x19); return 0; }
    uint32_t skill = e->call(0x1180, 2, unit + 0x20) ? sb : sa;
    uint32_t sid = e->call(0xA49C, 0, 0, 0, 0, 0, skill, 0, 0xB66);
    int32_t lvl = (int32_t)d2rd32(st + 0x1C);
    if (lvl <= 1) lvl = 1;
    int32_t pct = (int16_t)d2rd16(row + 0x62 + diff * 2) - 2 * lvl;
    pct = pct <= 5 ? 5 : (pct < 100 ? pct : 100);
    if (ctx6 && (int32_t)e->call(0x1180, 100, unit + 0x20) < pct) {
        skill = e->call(0xA3D0, 0, unit, 0, 0, 0, 0xFFFFFFFFu);
        sid = 0;
    }
    if (!e->call(0xA4890, sid, target, unit, 0, 0, game, skill, ctx)) {
        skill = skill == sb ? sa : sb;
        const uint32_t sid2 = e->call(0xA49C, 0, 0, 0, 0, 0, skill, 0, 0xB79);
        if (!e->call(0xA4890, sid2, target, unit, 0, 0, game, skill, ctx)) {
            skill = e->call(0xA3D0, 0, unit, 0, 0, 0, 0xFFFFFFFFu);
            if (!skill) {
                e->call(0xA9FA, 0, 0, 0, 0, 0, unit, 0, 1, 0);
                skill = e->call(0xA3D0, 0, unit, 0, 0, 0, 0xFFFFFFFFu);
            }
        }
    }
    if (!skill) { e->call(0xF22C0, game, 0, 0, 0, unit, 0x19); return 0; }
    const uint32_t id = e->call(0xA49C, 0, 0, 0, 0, 0, skill, 0, 0xB8F);
    uint32_t line = 0xBA8;
    if (e->call(0xAB7A, 0, 0, 0, 0, 0, unit, skill) == 1) {
        if (!ctx6) { e->call(0xF2540, 0, game, unit, 0, 0, ctx2, 0xF, 0, 0); return 0; }
        line = 0xB96;
    }
    const uint32_t id2 = e->call(0xA49C, 0, 0, 0, 0, 0, skill, 0, line);
    const uint32_t mode = e->call(0xAE1A, 0, 0, 0, 0, 0, skill);
    e->call(0xF2380, mode, unit, 0, game, 0, id2, ctx2, 0, 0);
    const uint32_t srow = e->call(0x1420, id);
    int32_t sl = (int32_t)e->call(0xA400, 0, 0, 0, 0, 0, unit, skill, 1);
    if (sl <= 1) sl = 1;
    const int32_t v = (int32_t)e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(srow + 0x190), id, (uint32_t)sl);
    d2wr32(d2rd32(ctx) + 0x14, (uint32_t)(v / 3 + (int32_t)d2rd32(game + 0xA8) + 0x12));
    return 0;
}
