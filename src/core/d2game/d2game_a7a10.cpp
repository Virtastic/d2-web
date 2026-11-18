// 1.13c D2Game+0xA7A10 (retail-layout port; its callees go through a0830d_env, see d2game_a0830d_env.h). Probe: src/core/fn/d2game_a7a10.cpp.
#include "d2game_a0830d_env.h"

// 1.13c D2Game+0xA7A10  void (ecx game, edx unit, stack ctx -- not read), ret 4
void D2GAME_A7A10(u8* game, int* unit, int* ctx)
{
    (void)ctx;
    u8 pk[8] = {0};                 // retail zeroes only the first dword; the target guid lands in it
    void* seed = unit + 8;
    if (FUN_6fc7a290(unit, pk, game)) {
        int target = at<int>(pk, 0);
        if (!target) { FUN_6fd122c0(0x19, game, unit); return; }
        if (!FUN_6fc79290(unit, game)) {
            int y = (int)FUN_6fc21130(P(target));
            int x = (int)FUN_6fc21100(P(target));
            if ((uint32_t)FUN_6fcd07b0(x, y, unit) >= 4) {
                seedStep(seed);
                if (at<uint32_t>(seed, 0) % 100 < 0x46) { FUN_6fd12540(game, unit, target, 2, 0, 0, 1, 7); return; }
                if ((int)FUN_6fc21180(seed, 100) < 10) { FUN_6fd11b10(game, unit, 4); return; }
                FUN_6fd122c0(10, game, unit); return;
            }
        }
        FUN_6fc79230(unit, game);
        Ordinal_10159(unit[0xb]);
        for (u8* ev = (u8*)(intptr_t)unit[0x37]; ev; ) {
            u8 kind = *ev;
            u8* next = (u8*)(intptr_t)at<int>(ev, 0x24);
            if (kind == 2) FUN_6fcae6f0(ev, game);
            ev = next;
        }
        Ordinal_10885(Ordinal_10331(unit, unit[0], unit[3]));
        Ordinal_10408(unit);
        Ordinal_11016(unit);
        FUN_6fd020a0(game, unit);
        return;
    }
    if (int* node = (int*)(intptr_t)FUN_6fd130c0(game, pk, unit)) {
        if (node[0] == 0) {
            if (FUN_6fc7a160(game, unit, node)) {
                FUN_6fc7b9b0(game, node, unit);
                if (unit[0x34] == 0xb) FUN_6fd0fdb0(unit, 0, 8, game);
            } else {
                FUN_6fc7a200(game);
                if ((int)FUN_6fc21180(seed, 1000) < 100) {
                    pk[0] = 0x8a; pk[1] = 1;
                    at<int>(pk, 2) = unit ? unit[3] : -1;
                    int client = node[0] == 0 ? at<int>(P(Ordinal_10920(node)), 0x9c) : 0;
                    FUN_6fcaa770(client, pk);
                }
            }
        }
    }
    FUN_6fd122c0(0x19, game, unit);
}
