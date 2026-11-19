// 1.13c D2Game+0xAA710 (retail-layout port; its callees go through a0830d_env, see d2game_a0830d_env.h). Probe: src/core/fn/d2game_aa710.cpp.
#include "d2game_a0830d_env.h"

static inline u8* monAi(int* u) { return (u[0] == 1 && u[5]) ? (u8*)P(at<int>(P(u[5]), 0x2c)) : nullptr; }

// 1.13c D2Game+0xAA710  void (stack game, ctx; eax unit), ret 8
// st = ctx[0]: +0x14 retreat flag, +0x18 retreat-from guid, +0x1C remembered target guid. ctx[7] parameter row.
// Both B1C90 out-param pairs alias frame slots: the first gets the game slot, the second the state slot.
void D2GAME_AA710(u8* game, int* ctx, int* unit)
{
    int* st = (int*)P(ctx[0]);
    u8* row = (u8*)P(ctx[7]);
    int* target = nullptr;
    if (unit && unit[0] == 1 && unit[5]) {
        int* s = (int*)P(at<int>(P(unit[5]), 0x28));
        if (s && s[0xa]) target = (int*)P(FUN_6fd003a0(s[0xa], s[0xb], s[0xc]));
    }
    if (!target) {
        int g = st[7];
        if (g) {
            if (!game) __builtin_trap();   // retail asserts (line 0x2d3)
            int* found = (int*)P(at<int>(game, 0x1120 + (g & 0x7f) * 4));
            while (found && found[3] != g) found = (int*)P(found[0x39]);
            if (at<int16_t>(row, 0x172) >= 0 && found) {
                int y = (int)FUN_6fc21130(found), x = (int)FUN_6fc21100(found);
                FUN_6fd12380(unit, (int)at<int16_t>(row, 0x172), 0, x, y, (int)at<u8>(row, 0x181), game);
                FUN_6fcaea80(game, unit, 0, at<int>(game, 0xa8) + 6, 0, 0, 0);
                FUN_6fd120c0(8, game, unit);
                return;
            }
        }
        FUN_6fd122c0(10, game, unit);
        return;
    }
    st[7] = target[3];
    if (Ordinal_10057(Ordinal_10331(unit))) {
        if (FUN_6fc3e140(game, unit, 0, 0, 0, 6, target)) return;
        FUN_6fd122c0(0x21, game, unit);
        return;
    }
    int thr = at<int16_t>(row, 0x68 + at<u8>(game, 0x6d) * 2);
    int o1 = (int)(intptr_t)game, o2 = 0, o3 = 0, stSlot = (int)(intptr_t)st;
    int r = (int)FUN_6fcd1c90(unit, st, &o1, &o2, game);
    int pick = (r && o1 <= thr) ? r : 0;
    int r2 = (int)FUN_6fcd1c90(target, st, &o3, &stSlot, game);
    if (!(pick && FUN_6fcd0e90(unit, pick))) {
        pick = 0;
        if (r2 && o3 < thr && FUN_6fcd0e90(unit, r2)) pick = r2;
    }
    int dist = (int)Ordinal_10522(target, unit);
    if (pick && dist > thr && (int)Ordinal_10522(target, (intptr_t)pick) > thr) pick = 0;
    int pct = 100;
    int16_t c = at<int16_t>(row, 0x32);
    if (c > 0) { pct = at<int16_t>(row, 0x34) * 100 / c - 100; if (pct >= 100) pct = 100; }
    if (at<int16_t>(row, 0x172) >= 0 && dist > 0x32) {
        int y = (int)FUN_6fc21130(target), x = (int)FUN_6fc21100(target);
        FUN_6fd12380(unit, (int)at<int16_t>(row, 0x172), 0, x, y, (int)at<u8>(row, 0x181), game);
        FUN_6fcaea80(game, unit, 0, at<int>(game, 0xa8) + 2, 0, 0, 0);
        FUN_6fd120c0(10, game, unit);
        return;
    }
    if (dist > at<int16_t>(row, 0x6e + at<u8>(game, 0x6d) * 2)) { FUN_6fc3cae0(game, target, 0, 1, 100, 0, unit); return; }
    if (dist > thr) {
        int mode = target[4];
        if (mode == 3) { FUN_6fc3cae0(game, target, 0, 1, 100, 0, unit); return; }
        if (mode == 2 || mode == 6) { FUN_6fc3cae0(game, target, 0, 0, 0, 0, unit); return; }
    }
    if (FUN_6fc3e140(game, unit, pick, o2, 1, 6, target)) return;
    intptr_t cursed = Ordinal_10494(unit, 0x8a);
    bool clearFlag = false;
    if (at<int16_t>(row, 0x170) >= 0 && !cursed) {
        bool tryFlee = (int)FUN_6fc21180(unit + 8, 100) < at<int16_t>(row, 0x62 + at<u8>(game, 0x6d) * 2)
                       || !pick || at<int>(P(ctx[0]), 0x14);
        if (tryFlee) {
            int* n = (int*)P(FUN_6fc2c1f0(game, 10, unit));
            if (n && (int)Ordinal_10522(n, unit) < thr / 2) {
                if (Ordinal_11138(unit, n, 0)) {
                    at<int>(P(ctx[0]), 0x14) = 0;
                    FUN_6fd12380(unit, (int)at<int16_t>(row, 0x170), (int)(intptr_t)n, 0, 0, (int)at<u8>(row, 0x180), game);
                    return;
                }
                if (!o2) {
                    FUN_6fd12540(game, unit, (int)(intptr_t)n, 0xf, 0, 0, 1, 0);
                    at<int>(P(ctx[0]), 0x14) = 1;
                    at<int>(P(ctx[0]), 0x18) = n[3];
                    return;
                }
            }
        } else clearFlag = true;
    } else clearFlag = true;
    if (clearFlag) at<int>(P(ctx[0]), 0x14) = 0;
    if (o2) {
        FUN_6fd12220(game, pick, 4, unit);
        FUN_6fd120c0((int)at<int16_t>(row, 0x56 + at<u8>(game, 0x6d) * 2), game, unit);
        return;
    }
    if (pick) {
        if (pct < -0x7e || pct > 0x7e) __builtin_trap();   // retail asserts (line 0x110)
        if (pct) at<int>(monAi(unit), 0x1c) = pct;
        FUN_6fd12540(game, unit, pick, 0xf, 0, 0, 1, 0);
        return;
    }
    if ((int)FUN_6fc21180(unit + 8, 100) < at<int16_t>(row, 0x5c + at<u8>(game, 0x6d) * 2)) FUN_6fd11b10(game, unit, 10);
    else FUN_6fd122c0(0xf, game, unit);
}
