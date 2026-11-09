// 1.13c D2Game+0xF14F0 -- item skill effect. EAX = target, ECX = unit, stdcall, ten stack words, ret 0x28: game, skill id, level,
// x, y, out0..out3 (pointers: kind / id pair and x / y pair), a9. dt = *sgptDataTables, gs = the module word at RVA 0xF8BE0 (a
// struct whose byte +4 is a mask). Skill record = dt[+0xB98] + id * 0x23C (0 when id is out of range); id 0 or a zero word
// at record+0x16A returns 0. Otherwise: 0xE07A0 (EAX unit, ECX game), the caster's owner via 0xA598 (0 if it is the unit itself),
// the unit's position (0xAAC6 / 0xAAD8) is saved, and the unit is placed on the target (0xACD0) or at x / y (0xAB2C). The out pairs
// are set to {6, -1} / {0, 0}, then record byte +0x120 picks: 1 target = self (0xACD0), out0/out1 = unit type / id; 2 0xF12C0 (0
// jumps to the cleanup), out2/out3 = position; 3 0xF1490 (ECX game; skill, level, w7); 4 0xF1420 (EAX game; w7). When
// gs[4] & record[8] the check 0x9FD00 must pass; then 0xA19A0 runs the skill and its result is returned. The unit is moved back
// (0xACD0 to the owner or 0xAB2C to the saved position) and flag 0x40 of unit+0xC4 restored.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_f14f0(uint32_t target, uint32_t unit, uint32_t game, uint32_t id, uint32_t level, uint32_t x, uint32_t y,
                              uint32_t out0, uint32_t out1, uint32_t out2, uint32_t out3, uint32_t a9, uint32_t dt, uint32_t gs,
                              const D2Seam* e)
{
    uint32_t rec = 0;
    if ((int32_t)id >= 0 && (int32_t)id < (int32_t)d2rd32(dt + 0xBA0)) rec = id * 0x23C + d2rd32(dt + 0xB98);
    if (id == 0 || d2rd16(rec + 0x16A) == 0) return 0;
    e->call(0xE07A0, unit, game);
    const uint32_t pos = d2rd32(unit + 0x2C);
    const uint32_t ow = e->call(0xA598, 0, 0, 0, 0, 0, pos);
    const uint32_t owner = ow == unit ? 0 : ow;
    const uint32_t px = e->call(0xAAC6, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C));
    const uint32_t py = e->call(0xAAD8, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C));
    const uint32_t had = (d2rd32(unit + 0xC4) >> 6) & 1;
    if (target) e->call(0xACD0, 0, 0, 0, 0, 0, unit, target);
    else e->call(0xAB2C, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C), x, y);
    if (out0) { d2wr32(out0, 6); if (out1) d2wr32(out1, 0xffffffffu); }
    if (out2) { d2wr32(out2, 0); if (out3) d2wr32(out3, 0); }
    uint32_t result = 0;
    bool run = true;
    switch (d2rd8(rec + 0x120)) {
    case 1:
        e->call(0xACD0, 0, 0, 0, 0, 0, unit, unit);
        if (out0) { d2wr32(out0, d2rd32(unit)); if (out1) d2wr32(out1, d2rd32(unit + 0xC)); }
        break;
    case 2:
        if (!e->call(0xF12C0, 0, 0, 0, 0, 0, unit)) { run = false; break; }
        if (out2) d2wr32(out2, e->call(0xAAC6, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C)));
        if (out3) d2wr32(out3, e->call(0xAAD8, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C)));
        break;
    case 3:
        if (!e->call(0xF1490, 0, game, 0, 0, 0, id, level, out1)) run = false;
        break;
    case 4:
        if (!e->call(0xF1420, game, 0, 0, 0, 0, out1)) run = false;
        break;
    default: break;
    }
    if (run) {
        if (!(d2rd8(gs + 4) & d2rd8(rec + 8)) || e->call(0x9FD00, id, 0, 0, 0, 0, game, unit, level, 1)) {
            const uint32_t a[6] = {game, id, level, 0, 1, a9};
            result = e->call(0xA19A0, unit, 0, 0, 0, 0, a[0], a[1], a[2], a[3], a[4], a[5]);
            e->blk(0xA19A0, a, 6);
        }
    }
    if (owner) e->call(0xACD0, 0, 0, 0, 0, 0, unit, owner);
    else e->call(0xAB2C, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C), px, py);
    d2wr32(unit + 0xC4, had ? d2rd32(unit + 0xC4) | 0x40 : d2rd32(unit + 0xC4) & ~0x40u);
    return result;
}
