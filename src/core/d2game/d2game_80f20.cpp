// 1.13c D2Game+0x80F20 -- the willowisp AI tick (monster AI function 25). Fastcall: ECX = game, EDX = unit, one stack word pAiTickParam
// (ret 4). The tick param holds +0 aiControl, +8 target, +0x18 combat flag, +0x1C monster parameters (u16 per difficulty at +0x56 cast,
// +0x5C melee, +0x62 approach). aiControl +0x14 = state, +0x18 = counter, +0x1C = slot (cooldown frame while idle).
// States: 0/2/3 not moving, 1 moving, 4 look for a formation, 5 baptism (slots 1-5), 6-12 ritual (slots 1-4). Void; EAX is residue.
// Callees go through the seam: 0x1900 squared distance (EAX unit, EBX target), 0x1180 roll (EAX range, ECX = unit+0x20), 0x1100 / 0x1130
// unit x / y (ECX), 0xB07B0 distance to a point, 0xF1B10 walk close (AL), 0xF2540 walk to target, 0xF2220 / 0xF21E0 / 0xF1FA0 mode and
// coordinate moves, 0xF22C0 idle, 0xDFA90 is-dead, 0xA478 / 0xA454 / 0xA3B8 / 0xA502 / 0xA4F0 / 0xA4FC the room unit find, and
// 0xA6A0 / 0xA6E2 / 0xA7C6 / 0x8EA80 / 0xA60A the magic-find stat list. The ritual and baptism offset tables are the stack
// constants the function builds on entry (1.13c keeps them as immediates, in the same order as the 1.10f statics).
#include "d2game_seam4e.h"

namespace {
struct Pt { int32_t x, y; };
// ritual: [shape][whisp] position about the target and the aim offset, shapes 6..12, whisps 1..4
const Pt kRitPos[7][4] = {
    {{-10, -3}, {1, -5}, {6, 0}, {4, 10}},     {{-9, -2}, {-6, -11}, {-2, -5}, {6, 1}},   {{0, 13}, {-12, 0}, {7, 5}, {-3, -11}},
    {{-15, -3}, {-6, -3}, {-4, -5}, {-6, -13}}, {{-12, -2}, {-6, 2}, {-5, 5}, {1, 11}},     {{-13, -6}, {-8, -11}, {-7, 0}, {1, 8}},
    {{-5, -8}, {-8, 1}, {1, -6}, {6, 9}}};
const Pt kRitAim[7][4] = {
    {{1, -5}, {6, 0}, {4, 10}, {-10, -3}},     {{1, 8}, {-9, -2}, {-5, 4}, {1, 8}},      {{-12, 0}, {7, 5}, {-3, -11}, {11, -2}},
    {{7, 9}, {-15, -3}, {-6, -13}, {9, 7}},    {{-5, -7}, {-5, -7}, {9, 0}, {-12, -2}},  {{-8, -11}, {-2, -6}, {-2, -6}, {-13, -6}},
    {{-8, 1}, {1, -6}, {6, 9}, {0, 0}}};
// baptism: slots 1..5
const Pt kBapPos[5] = {{-5, -5}, {3, -5}, {6, 3}, {3, 6}, {-5, 3}};
const Pt kBapAim[5] = {{6, 3}, {3, 6}, {-5, 3}, {-5, -5}, {3, -5}};
}  // namespace

extern "C" void d2g_80f20(uint32_t game, uint32_t unit, uint32_t tick, const D2Seam* e)
{
    const uint32_t ai = d2rd32(tick), target = d2rd32(tick + 8), combat = d2rd32(tick + 0x18), prm = d2rd32(tick + 0x1C);
    const uint32_t diff = d2rd8(game + 0x6D);
    const int32_t frame = (int32_t)d2rd32(game + 0xA8);
    auto roll = [&](uint32_t range) { return (int32_t)e->call(0x1180, range, unit + 0x20); };
    auto idle = [&](uint32_t who, int32_t frames) { e->call(0xF22C0, game, 0, 0, 0, who, (uint32_t)frames); };
    auto mode = [&](uint32_t m, uint32_t tgt) { e->call(0xF2220, m, 0, 0, 0, 0, game, tgt); e->edi(0xF2220, unit); };
    auto walkClose = [&](uint32_t dist) { e->call(0xF1B10, dist, 0, 0, 0, 0, game, unit); };
    auto walkTo = [&]() {
        e->call(0xF2540, 0, game, unit, 0, 0, target, 2, 0, 0, 1, 0);
        const uint32_t w[6] = {target, 2, 0, 0, 1, 0};
        e->blk(0xF2540, w, 6);
    };
    auto tgtX = [&]() { return e->call(0x1100, 0, target); };
    auto tgtY = [&]() { return e->call(0x1130, 0, target); };
    auto setParam = [&](uint32_t u, uint32_t off, uint32_t v) {   // the guarded monster ai-control write
        if (!u || d2rd32(u) != 1) return;
        const uint32_t m = d2rd32(u + 0x14);
        if (!m) return;
        const uint32_t a = d2rd32(m + 0x28);
        if (a) d2wr32(a + off, v);
    };
    const uint32_t cdown = (uint32_t)frame + 0x708;

    if (target && frame > (int32_t)d2rd32(ai + 0x1C) && (int32_t)e->call(0x1900, unit, 0, 0, target) < 0x400 &&
        (int32_t)d2rd32(ai + 0x14) < 4 && roll(1000) <= (int32_t)diff + 2)
        d2wr32(ai + 0x14, 4);

    int32_t state = (int32_t)d2rd32(ai + 0x14);
    if (state == 4) {
        const uint32_t room = e->call(0xA478, 0, 0, 0, 0, 0, unit);
        const uint32_t type = d2rd32(unit), path = d2rd32(unit + 0x2C);
        const bool onPath = type == 2 || (type > 3 && type < 6);
        const uint32_t x = onPath ? d2rd32(path + 0xC) : path ? e->call(0xA454, 0, 0, 0, 0, 0, path) : 0;
        const uint32_t y = onPath ? d2rd32(path + 0x10) : path ? e->call(0xA3B8, 0, 0, 0, 0, 0, path) : 0;
        uint32_t query[14] = {}, result[8] = {};
        query[0] = 0x583; query[3] = x; query[4] = y; query[5] = 0x20;
        const uint32_t q = (uint32_t)(uintptr_t)query, res = (uint32_t)(uintptr_t)result;
        e->call(0xA502, 0, 0, 0, 0, 0, d2rd32(game + 0x1C), res, room, x, y, 0x20, 0x6FC9BD00, q);
        e->blk(0xA502, query, 14);
        e->call(0xA4F0, 0, 0, 0, 0, 0, res);
        const int32_t count = (int32_t)result[6];
        const uint32_t list = result[1];
        if (count == 4 || count == 5) {
            for (int32_t i = 0; i < count; ++i) {
                const uint32_t u = d2rd32(list + 4 * i);
                setParam(u, 0x14, count == 4 ? 6 : 5);
                setParam(u, 0x18, 0);
                setParam(u, 0x1C, (uint32_t)i + 1);
                idle(d2rd32(list + 4 * i), 0x151 - frame % 0x151);
            }
            idle(unit, 0x151 - frame % 0x151);
        } else {
            d2wr32(ai + 0x14, 2);
            idle(unit, 8);
        }
        e->call(0xA4FC, 0, 0, 0, 0, 0, res);
        return;
    }

    if (state >= 6) {   // ritual: shape = state, whisp = slot
        const int32_t slot = (int32_t)d2rd32(ai + 0x1C);
        if (slot > 4) { d2wr32(ai + 0x14, 2); idle(unit, 8); return; }
        const bool ok = slot >= 1 && state <= 12;   // slots 0 and shapes past 12 read stack the function never wrote
        const Pt pos = ok ? kRitPos[state - 6][slot - 1] : Pt{0, 0}, aim = ok ? kRitAim[state - 6][slot - 1] : Pt{0, 0};
        const int32_t px = (int32_t)tgtX() + pos.x;
        const int32_t py = (int32_t)tgtY() + pos.y;
        if (e->call(0xB07B0, unit, 0, 0, 0, 0, (uint32_t)px, (uint32_t)py) > 3) {
            if (roll(100) < 0x22) walkClose(4);
            else e->call(0xF1FA0, 2, unit, 0, 0, 0, game, (uint32_t)px, (uint32_t)py);
            return;
        }
        if (frame % 0x43) { idle(unit, 0x43 - frame % 0x43); return; }
        const int32_t cnt = (int32_t)d2rd32(ai + 0x18);
        if (cnt < 3) {
            if (!aim.x && !aim.y) {
                d2wr32(ai + 0x18, (uint32_t)cnt + 1);
                idle(unit, 0x43 - frame % 0x43);
                return;
            }
            const int32_t ax = (int32_t)tgtX() + aim.x;
            const int32_t ay = (int32_t)tgtY() + aim.y;
            e->call(0xF21E0, 7, 0, 0, 0, 0, game, (uint32_t)ax, (uint32_t)ay);
            e->edi(0xF21E0, unit);
            d2wr32(ai + 0x18, d2rd32(ai + 0x18) + 1);
            return;
        }
        const int32_t ns = state + 1;
        d2wr32(ai + 0x14, (uint32_t)ns);
        d2wr32(ai + 0x18, 0);
        if (ns - 6 > 6) { d2wr32(ai + 0x14, 2); d2wr32(ai + 0x1C, cdown); }
        idle(unit, 0x151 - frame % 0x151);
        if (target && d2rd32(target) == 0 && e->call(0xDFA90, target) == 0) {
            const uint32_t id = d2rd32(target + 0xC), expiry = (uint32_t)frame + 0x1A5E00;
            const uint32_t sl = e->call(0xA6A0,0, 0, 0, 0, 0, d2rd32(game + 0x1C), 2, expiry, 0, id);
            if (sl) {
                e->call(0xA6E2,0, 0, 0, 0, 0, sl, 0x50, (diff + 1) * 0x32, 0);
                e->call(0xA7C6,0, 0, 0, 0, 0, sl, expiry);
                e->call(0x8EA80, 0, game, target, 0, 0, 0xC, expiry, 0, 0, 0);
                e->call(0xA60A,0, 0, 0, 0, 0, target, sl, 1);
            }
        }
        return;
    }

    if (state == 5) {   // baptism: slot = whisp 1..5
        const int32_t slot = (int32_t)d2rd32(ai + 0x1C);
        if (slot > 5) { d2wr32(ai + 0x14, 2); idle(unit, 8); return; }
        const bool ok = slot >= 1;
        const Pt pos = ok ? kBapPos[slot - 1] : Pt{0, 0}, aim = ok ? kBapAim[slot - 1] : Pt{0, 0};
        const int32_t px = (int32_t)tgtX() + pos.x;
        const int32_t py = (int32_t)tgtY() + pos.y;
        if (e->call(0xB07B0, unit, 0, 0, 0, 0, (uint32_t)px, (uint32_t)py) > 3) {
            if (roll(100) < 0x22) walkClose(4);
            else e->call(0xF1FA0, 2, unit, 0, 0, 0, game, (uint32_t)px, (uint32_t)py);
            return;
        }
        if (frame % 0x43) { idle(unit, 0x43 - frame % 0x43); return; }
        const int32_t cnt = (int32_t)d2rd32(ai + 0x18);
        if (cnt >= 3) {
            d2wr32(ai + 0x14, 2);
            d2wr32(ai + 0x1C, cdown);
            idle(unit, 0x151 - frame % 0x151);
            return;
        }
        if (!aim.x && !aim.y) {
            d2wr32(ai + 0x18, (uint32_t)cnt + 1);
            idle(unit, 0x151 - frame % 0x151);
            return;
        }
        const int32_t ax = (int32_t)tgtX() + aim.x;
        const int32_t ay = (int32_t)tgtY() + aim.y;
        e->call(0xF21E0, 7, 0, 0, 0, 0, game, (uint32_t)ax, (uint32_t)ay);
        e->edi(0xF21E0, unit);
        d2wr32(ai + 0x18, d2rd32(ai + 0x18) + 1);
        return;
    }

    if (state == 1) {   // moving
        const int32_t cnt = (int32_t)d2rd32(ai + 0x18);
        if (cnt <= 0) {
            if (combat) { mode(8, 0); d2wr32(ai + 0x14, 3); return; }
            if (roll(100) < (int16_t)d2rd16(prm + diff * 2 + 0x56)) { mode(8, 0); d2wr32(ai + 0x14, 2); return; }
        }
        d2wr32(ai + 0x18, d2rd32(ai + 0x18) - 1);
        const int32_t r = roll(100);
        d2wr32(ai + 0x14, 1);
        if (r < (int16_t)d2rd16(prm + diff * 2 + 0x62)) walkTo();
        else walkClose(6);
        return;
    }

    // states 0, 2, 3 (and anything else below 6)
    if (!combat) {
        if (state == 2 || roll(100) < (int16_t)d2rd16(prm + diff * 2 + 0x56)) { mode(7, target); d2wr32(ai + 0x14, 0); return; }
    } else if (state == 3 || roll(100) < (int16_t)d2rd16(prm + diff * 2 + 0x5C)) {
        mode(4, target); d2wr32(ai + 0x14, 0); return;
    }
    const int32_t r = roll(100);
    d2wr32(ai + 0x14, 1);
    if (r < (int16_t)d2rd16(prm + diff * 2 + 0x62)) walkTo();
    else walkClose(4);
    d2wr32(ai + 0x18, 3);
}
