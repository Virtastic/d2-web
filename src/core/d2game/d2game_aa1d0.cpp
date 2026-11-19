// 1.13c D2Game+0xAA1D0 (retail-layout port; its callees go through a0830d_env, see d2game_a0830d_env.h). Probe: src/core/fn/d2game_aa1d0.cpp.
#include "d2game_a0830d_env.h"

// 0x01A90 inlined: skills row +0x158 + (lvl - 1) * row +0x15C; 0 for a bad id or lvl <= 0.
static inline int skillLevelValue2(int id, int lvl)
{
    if (id < 0 || id >= at<int>(tables(), 0xba0) || lvl <= 0) return 0;
    u8* r = (u8*)P(at<int>(tables(), 0xb98)) + id * 0x23c;
    if (!r) return 0;
    return at<int>(r, 0x158) + (lvl - 1) * at<int>(r, 0x15c);
}

// 1.13c D2Game+0xAA1D0  void (ecx game, edx unit, stack ctx), ret 4
// st = ctx[0]: +0x14 charge counter (-1 = unset), +0x18 next-use frame, +0x1C toggle. ctx[7] parameter row.
void D2GAME_AA1D0(u8* game, int* unit, int* ctx)
{
    int* st = (int*)P(ctx[0]);
    int* target = nullptr;
    if (unit && unit[0] == 1 && unit[5]) {
        int* s = (int*)P(at<int>(P(unit[5]), 0x28));
        if (s && s[0xa]) target = (int*)P(FUN_6fd003a0(s[0xa], s[0xb], s[0xc]));
    }
    u8* row = target ? (u8*)P(ctx[7]) : nullptr;
    if (!row) { FUN_6fd122c0(10, game, unit); return; }
    if (st[5] == -1) {
        st[5] = 3;
        int16_t sk = at<int16_t>(row, 0x170);
        if (sk >= 0) {
            int lvl = (int)Ordinal_10306(target, Ordinal_10630(target, (int)sk), 1);
            st[5] = skillLevelValue2(at<int16_t>(row, 0x170), lvl);
        }
    } else if (st[5] == 0) {
        FUN_6fcfeee0(game, unit, unit, 1);
        return;
    }
    int dist = (int)FUN_6fcd0a20(target, unit);
    if (dist > 0x32) { FUN_6fc3cae0(game, target, 3, 0, 0, 0, unit); return; }
    int pct = 100;
    int16_t c = at<int16_t>(row, 0x32);
    if (c > 0) { pct = at<int16_t>(row, 0x34) * 100 / c - 100; if (pct >= 100) pct = 100; }
    if (dist > 0x1c) { FUN_6fc3cae0(game, target, 0, 0, pct, 0, unit); return; }
    int o1 = 0, o2 = 0;
    if (int r = (int)FUN_6fcd1c90(target, ctx[0], &o1, &o2, game)) Ordinal_10522(unit, r);
    int c6 = ctx[6], c5 = ctx[5], c2 = ctx[2];
    int d = at<u8>(game, 0x6d);
    int avg = (at<int16_t>(row, 0x5c + d * 2) + at<int16_t>(row, 0x56 + d * 2)) / 2;
    if (c2 && st[6] < at<int>(game, 0xa8)) {
        if ((int)FUN_6fc21180(unit + 8, 100) < at<int16_t>(row, 0x68 + d * 2) && c5 < at<int16_t>(row, 0x6e + d * 2)) {
            if (c6) {
                FUN_6fd12220(game, c2, 4, unit);
                int dd = at<u8>(game, 0x6d);
                st[6] = at<int>(game, 0xa8) + at<int16_t>(row, 0x62 + dd * 2) * 10;
                st[5] -= 1;
            } else {
                FUN_6fd12540(game, unit, c2, 2, 0, 0, 1, 0);
            }
            return;
        }
    }
    d = at<u8>(game, 0x6d);
    if (dist <= at<int16_t>(row, 0x56 + d * 2) && dist >= at<int16_t>(row, 0x5c + d * 2)) {
        if (FUN_6fcc41b0(4, game, target, st[7] ? 5 : 6, unit)) return;
        st[7] = st[7] == 0;
        if (FUN_6fcc41b0(4, game, target, st[7] ? 5 : 6, unit)) return;
    }
    if (!FUN_6fd11c30(game, avg, target, unit)) FUN_6fc3cae0(game, target, 1, 0, 0, 0, unit);
}
