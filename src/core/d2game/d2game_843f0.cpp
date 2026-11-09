// 1.13c D2Game+0x843F0: AI think step that raises a minion before fighting (fastcall: ECX = game, EDX = unit, stack = AI tick record,
// ret 4). Record +0 control (+0x14 = minions raised), +8 target, +0x18 combat flag, +0x1C MonStats row; row words by game+0x6D:
// +0x56 raise limit, +0x5C nearby limit, +0x62 raise roll, +0x68 command roll, +0x6E stand roll, +0x74 wander roll, +0x7A idle
// frames.
#include <cstdint>
#include <emscripten.h>
#include "d2game_aiext_env.h"
using namespace aiext;

// 1.13c D2Game+0x843F0
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_843f0_port(const D2Game77dEnv* e, const D2GameAiExt* x, uint32_t game, uint32_t unit, uint32_t task) {
    const uint32_t ctl = rd32(task, 0), target = rd32(task, 8), row = rd32(task, 0x1c), flag = rd32(task, 0x18);
    const uint32_t cls = x->th2ACBE(unit == 0 ? 0xffffffffu : rd32(unit, 4), 0, 0);
    const uint32_t tables = rd32(x->dataTablesSlot(), 0);
    const uint32_t kind = x->sub1D30((int32_t)rd32(tables, 0xa80) > 0x12d ? 0x12d : 0xffffffffu, cls);
    uint32_t d = rd8(game, 0x6d) * 2u;
    if ((int32_t)rd32(ctl, 0x14) < rdS16(row, 0x56 + d) && x->th2A514(unit) == 0) {
        d = rd8(game, 0x6d) * 2u;
        if ((int32_t)roll100(unit) < rdS16(row, 0x62 + d)) {
            uint32_t buf[3] = { kind, 0x19, 0 };
            e->subB17B0m(0xc, (uint32_t)(uintptr_t)buf, game, unit, 0);
            d = rd8(game, 0x6d) * 2u;
            if ((int32_t)buf[2] < rdS16(row, 0x5c + d) && x->sub7C6E0(target, unit, game, row) != 0) {
                wr32(ctl, 0x14, rd32(ctl, 0x14) + 1);
                return;
            }
        }
    }
    d = rd8(game, 0x6d) * 2u;
    if (flag != 0) {
        if ((int32_t)roll100(unit) < rdS16(row, 0x68 + d)) { postCmd(e, game, unit, target, 4, 0x10); return; }
        e->subF22C0(game, (uint32_t)rdS16(row, 0x7a + d));
        return;
    }
    if ((int32_t)rd32(ctl, 0x14) < rdS16(row, 0x56 + d) && (int32_t)roll100(unit) >= rdS16(row, 0x6e + d)) {
        if ((int32_t)roll100(unit) < rdS16(row, 0x74 + d)) {
            wander(unit, 4);
            e->subF2540(game, unit, target, 2, 0, 0, 1, 0);
            return;
        }
        e->subF22C0(game, 0xf);
        return;
    }
    e->subF2540(game, unit, target, 2, 0, 0, 1, 7);
}
