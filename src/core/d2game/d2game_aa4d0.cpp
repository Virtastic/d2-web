// 1.13c D2Game+0xAA4D0 (retail-layout port; its callees go through a0830d_env, see d2game_a0830d_env.h). Probe: src/core/fn/d2game_aa4d0.cpp.
#include "d2game_a0830d_env.h"

static inline bool staticPath(int* u) { return u[0] == 2 || (u[0] > 3 && u[0] <= 5); }
static inline int unitPX(int* u) { return staticPath(u) ? at<int>(P(u[0xb]), 0xc) : (u[0xb] ? (int)Ordinal_10867(u[0xb]) : 0); }
static inline int unitPY(int* u) { return staticPath(u) ? at<int>(P(u[0xb]), 0x10) : (u[0xb] ? (int)Ordinal_10750(u[0xb]) : 0); }

// 1.13c D2Game+0xAA4D0  void (ecx game, edx unit, stack ctx), ret 4
void D2GAME_AA4D0(u8* game, int* unit, int* ctx)
{
    int* target = nullptr;
    if (unit && unit[0] == 1 && unit[5]) {
        int* s = (int*)P(at<int>(P(unit[5]), 0x28));
        if (s && s[0xa]) target = (int*)P(FUN_6fd003a0(s[0xa], s[0xb], s[0xc]));
    }
    if (!target) { FUN_6fd122c0(10, game, unit); return; }
    u8* row = (u8*)P(ctx[7]);
    int o1 = (int)(intptr_t)ctx, o2 = 0;   // o1 is the frame slot of ctx; o2 an uninitialised local
    int r = (int)FUN_6fcd1c90(unit, ctx[0], &o1, &o2, game);
    if (r && o1 > 0x18) r = 0;
    if (!r) r = 0;
    if (o2 && r) {
        seedStep(unit + 8);
        if ((int)(at<uint32_t>(unit + 8, 0) % 100) < at<int16_t>(row, 0x56 + at<u8>(game, 0x6d) * 2)
            && FUN_6fd12d80(game, unit, 6, 1, r)) return;
    }
    seedStep(unit + 8);
    if ((int)(at<uint32_t>(unit + 8, 0) % 100) < at<int16_t>(row, 0x5c + at<u8>(game, 0x6d) * 2)) { r = 0; o2 = 0; }
    int dist = (int)Ordinal_10522(unit, target);
    if (dist > at<int16_t>(row, 0x62 + at<u8>(game, 0x6d) * 2)) {
        int y = unitPY(target), x = unitPX(target);
        if (FUN_6fd019c0(x, Ordinal_10331(target), game, unit, 0, 0, y)) { FUN_6fd122c0(0x19, game, unit); return; }
    }
    int mode = target[4];
    if (dist > at<int16_t>(row, 0x68 + at<u8>(game, 0x6d) * 2)) {
        if ((mode == 2 || mode == 6) && FUN_6fc3cae0(game, target, 0, 0, 0, 0, unit)) return;
        if (mode == 3 && FUN_6fc3cae0(game, target, 0, 0, 0x3c, 0, unit)) return;
    }
    if (FUN_6fc3e140(game, unit, r, o2, 0, 6, target)) return;
    FUN_6fd122c0(0x19, game, unit);
}
