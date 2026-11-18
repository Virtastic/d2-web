// 1.13c D2Game+0xA9E80 (retail-layout port; its callees go through a0830d_env, see d2game_a0830d_env.h). Probe: src/core/fn/d2game_a9e80.cpp.
#include "d2game_a0830d_env.h"

// 1.13c D2Game+0xA9E80  void (ecx game, edx unit, stack ctx), ret 4
// ctx[0] is the AI state (+0x1C remembered target guid), ctx[7] the per-difficulty parameter row. The two
// out-param slots of B1C90 are the frame slots of ctx and of the state pointer, so both start from those values.
void D2GAME_A9E80(u8* game, int* unit, int* ctx)
{
    int* st = (int*)P(ctx[0]);
    int* target = nullptr;
    if (unit && unit[0] == 1 && unit[5]) {
        int* s = (int*)P(at<int>(P(unit[5]), 0x28));
        if (s && s[0xa]) target = (int*)P(FUN_6fd003a0(s[0xa], s[0xb], s[0xc]));
    }
    if (!target) {
        int g = st[7];
        if (g) {
            if (!game) __builtin_trap();   // retail asserts (line 0x2d3)
            for (int* u = (int*)P(at<int>(game, 0x1120 + (g & 0x7f) * 4)); u; u = (int*)P(u[0x39]))
                if (u[3] == g) { FUN_6fc3cae0(game, u, 3, 0, 0, 0, unit); return; }
        }
        FUN_6fd122c0(10, game, unit);
        return;
    }
    st[7] = target[3];
    int dist = (int)FUN_6fcd0a20(target, unit);
    int mode = target[4];
    u8* row = (u8*)P(ctx[7]);
    if (dist > 0x32) { FUN_6fc3cae0(game, target, 3, 0, 0, 0, unit); return; }
    int pct = 100;
    int16_t c = at<int16_t>(row, 0x32);
    if (c > 0) { pct = at<int16_t>(row, 0x34) * 100 / c - 100; if (pct >= 100) pct = 100; }
    if (dist > 0x1c) { FUN_6fc3cae0(game, target, 0, 0, 100, 0, unit); return; }
    if (dist > 0x12) {
        if ((mode == 2 || mode == 6) && FUN_6fc3cae0(game, target, 0, 0, 0, 0, unit)) return;
        if (mode == 3 && FUN_6fc3cae0(game, target, 0, 0, 100, 0, unit)) return;
    }
    int slotA = (int)(intptr_t)ctx, slotB = (int)(intptr_t)st;
    int r1 = (int)FUN_6fcd1c90(target, st, &slotB, &slotA, game);
    int r2 = (int)FUN_6fcd1c90(unit, st, &slotA, &slotB, game);
    bool picked = false;
    if (r2 && slotA <= 0x1c) {
        slotA = r2;
        if (FUN_6fcd0e90(unit, r2)) picked = true;
    }
    if (!picked) {
        slotA = 0;
        if (r1 && (int)FUN_6fcd0a20(P(r1), unit) < 0x1c && FUN_6fcd0e90(unit, r1)) slotA = r1;
    }
    int diff = at<u8>(game, 0x6d);
    if (slotB) {
        if (slotA) {
            if ((int)FUN_6fc21180(unit + 8, 100) < at<int16_t>(row, 0x62 + diff * 2)) {
                FUN_6fd12120(game, slotA, 0, 0, (int)at<int16_t>(row, 0x170));
                return;
            }
            FUN_6fd12220(game, slotA, 4, unit);
            FUN_6fd120c0((int)at<int16_t>(row, 0x56 + diff * 2), game, unit);
            return;
        }
    } else if (slotA) {
        if ((int)FUN_6fc21180(unit + 8, 100) < at<int16_t>(row, 0x5c + diff * 2)) {
            if (pct < -0x7e || pct > 0x7e) __builtin_trap();   // retail asserts (line 0x110)
            u8* ai = (unit[0] == 1 && unit[5]) ? (u8*)P(at<int>(P(unit[5]), 0x2c)) : nullptr;
            if (pct) at<int>(ai, 0x1c) = pct;
            at<u8>(ai, 0x20) = 0x28;
        }
        FUN_6fd12540(game, unit, slotA, 2, 0, 0, 1, 0);
        return;
    }
    if (dist > 0x10) FUN_6fc3cae0(game, target, 0, 0, 0, 0, unit);
    else FUN_6fd122c0(0xf, game, unit);
}
