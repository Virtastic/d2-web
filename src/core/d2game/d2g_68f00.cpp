// 1.13c D2Game+0x68F00 -- EDI = unit, EBX = cb {+4 game, +0x18 data}. Written from the 1.13c disassembly.
// By the unit's mode (+0x10; none = 0): 1 sets mode 2, 3 sets mode 4, 2 asks 0xA556(0xA478(unit)): 1 counts data+0x80 (past 5
// latches +0x92) and a latched +0x92 sets mode 3; otherwise a set +0x91 sets mode 3. Mode changes go through 0xA4A2(unit, m).
// Always ends with event 7 at frame + 0x19 (0x8EA80, ECX game, EDX unit); its EAX is the result.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t f68f00(HookEnv& e, uint32_t unit, uint32_t cb)
{
    const uint32_t mode = unit ? e.rd32(unit + 0x10) : 0;
    uint32_t m = 0;
    if (mode == 1) m = 2;
    else if (mode == 3) m = 4;
    else if (mode == 2) {
        const uint32_t s = e.rd32(cb + 0x18);
        const uint32_t st = e.h(0xA478, 0, 0, 0, 0, 0, unit);
        if (e.h(0xA556, 0, 0, 0, 0, 0, st) == 1) {
            const uint32_t c = e.rd32(s + 0x80) + 1;
            e.wr32(s + 0x80, c);
            if ((int32_t)c > 5) e.wr8(s + 0x92, 1);
            if (e.rd8(s + 0x92)) m = 3;
        } else if (e.rd8(s + 0x91)) m = 3;
    }
    if (m) e.h(0xA4A2, 0, 0, 0, 0, 0, unit, m);
    const uint32_t game = e.rd32(cb + 4);
    return e.h(0x8EA80, 0, game, unit, 0, 0, 7, e.rd32(game + 0xA8) + 0x19, 0, 0, 0);
}
}
