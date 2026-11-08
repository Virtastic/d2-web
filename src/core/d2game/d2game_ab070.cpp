// 1.13c D2Game+0xAB070 (sub_AB070, SkillUseOwnerTableGate): AI think callback for an owned monster that uses a skill on its owner's target. Registers: ECX = game,
// EDX = unit, stack = cmd (ret 4); cmd+0 is the AI control block (+0x18 the last-cast tick), +8 and +0x18 two words kept, +0x1C the monstats row. Written from the
// 1.13c disassembly over the seam environment (see d2game_addc0.cpp); the decompile mixes two locals, the disassembly is followed. Callees (RVA, regs, stack
// words): 0xE03A0 (EAX = [ex+0x30], ECX = [ex+0x28], EDX = [ex+0x2C]) answers the target; 0xAB86 (unit, other: the distance); 0x1CAE0 (EAX = unit; game,
// target, 3, 0, 0, 6); 0xDFA90 (EAX = word; a leaf that leaves EDX = 0); 0xA3F4 (ECX = unit), 0xA400 (unit, that, 1), 0xAC9A (unit, skill+0x64, id, that),
// 0xC1F0 (ESI = unit; game, clamp 5..50), 0xA754 (unit, obj, 0), 0x1E140 (game, unit, obj, word, 0, 6), 0xA42A (target, 6 or 8, 0), 0xA3A0 / 0xA4EA
// (target), 0xF2380 (EAX = (id & ~0xFF) | 8, ECX = unit; id, obj, 0, 0), 0x1180 (EAX = 100, ECX = unit+0x20), 0xF2D80 (EAX = word; game, unit, row byte, 0),
// 0xF2540 (ECX = game, EDX = unit; obj, 2, 0, 0, 1, 7), 0xF22C0 (EAX = game, ESI = unit; row word).
#include "../fn/d2game_gp.h"
namespace d2g {
namespace {
int32_t s16(GE& e, uint32_t a) { return (int16_t)(e.rd32(a & ~3u) >> ((a & 2) * 8)); }
}
void fab070(GE& e, uint32_t T, uint32_t game, uint32_t unit, uint32_t cmd)
{
    const uint32_t row = e.rd32(cmd + 0x1C), ai = e.rd32(cmd);
    if (!unit || e.rd32(unit) != 1) return;
    const uint32_t pd = e.rd32(unit + 0x14);
    if (!pd) return;
    const uint32_t ex = e.rd32(pd + 0x28);
    if (!ex || !e.rd32(ex + 0x28)) return;
    const uint32_t tg = e.h(0xE03A0, e.rd32(ex + 0x30), e.rd32(ex + 0x28), e.rd32(ex + 0x2C));
    if (!tg) return;
    uint32_t C = e.rd32(cmd + 8), D = e.rd32(cmd + 0x18), obj = 0, A = 0;
    int32_t B = 0, F = 1;
    const int32_t dist = (int32_t)e.h(0xAB86, 0, 0, 0, 0, 0, unit, tg);
    const uint32_t cls = e.rd8(game + 0x6D);
    if (dist >= s16(e, row + 0x6E + cls * 2))
        if (e.h(0x1CAE0, unit, 0, 0, 0, 0, game, tg, 3, 0, 0, 6)) return;
    if (C && e.h(0xDFA90, C)) { C = 0; D = 0; }
    const int32_t sk = s16(e, row + 0x170);
    if (sk > 0) {
        const uint32_t skrow = e.skillRec(T, sk);
        const uint32_t x = e.h(0xA3F4, 0, unit);
        if (skrow && x) {
            const uint32_t t1 = e.h(0xA400, 0, 0, 0, 0, 0, unit, x, 1);
            int32_t v = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(skrow + 0x64), (uint32_t)sk, t1);
            if (v <= 5) v = 5; else if (v >= 0x32) v = 0x32;
            obj = e.h(0xC1F0, 0, 0, 0, 0, unit, game, (uint32_t)v);
            if (obj) B = (int32_t)e.h(0xAB86, 0, 0, 0, 0, 0, unit, obj);
        }
    }
    if (B >= s16(e, row + 0x5C + cls * 2)) obj = 0;
    else if (obj) A = e.h(0xA754, 0, 0, 0, 0, 0, unit, obj, 0);
    if (e.h(0x1E140, 0, 0, 0, 0, 0, game, unit, obj, A, 0, 6)) return;
    const uint32_t kind = e.rd32(unit + 4);
    if (kind == 0x1AA || kind == 0x1AB) {
        const int32_t r1 = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, tg, kind == 0x1AA ? 6 : 8, 0);
        const int32_t r2 = (int32_t)e.h(kind == 0x1AA ? 0xA3A0 : 0xA4EA, 0, 0, 0, 0, 0, tg);
        if (r1 >= r2) F = 0;
    }
    if (obj && A && F && s16(e, row + 0x56 + cls * 2) + (int32_t)e.rd32(ai + 0x18) < (int32_t)e.rd32(game + 0xA8)) {
        e.h(0xF2380, ((uint32_t)sk & ~0xFFu) | 8, unit, 0, game, 0, (uint32_t)sk, obj, 0, 0);
        e.wr32(ai + 0x18, e.rd32(game + 0xA8));
        return;
    }
    if (D && C && (int32_t)e.h(0x1180, 0x64, unit + 0x20) < 0x19) {
        e.h(0xF2D80, C, 0, 0, 0, 0, game, unit, e.rd8(row + cls * 2 + 0x68), 0);
        return;
    }
    if (obj) { e.h(0xF2540, 0, game, unit, 0, 0, obj, 2, 0, 0, 1, 7); return; }
    e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)s16(e, row + 0x62 + cls * 2));
}
}
