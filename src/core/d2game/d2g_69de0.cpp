// 1.13c D2Game+0x69DE0 -- ClampHpToMaxAndClearStates (stdcall, unit, byte* flag). Written from the 1.13c disassembly.
// hp = 0xA42A(unit, 6, 0), max = 0xA3A0(unit); hp < max (signed) sets stat 6 := max through 0xA472(unit, 6, max, 0) and *flag := 1.
// States 2 then 1: a non-zero 0xA5D4(unit, s) node goes to 0xABB6(unit, node) and 0xABAA(node). The result is 0xBC920()'s.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t f69de0(HookEnv& e, uint32_t unit, uint32_t flag)
{
    const int32_t hp = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, unit, 6, 0);
    const int32_t mx = (int32_t)e.h(0xA3A0, 0, 0, 0, 0, 0, unit);
    if (hp < mx) {
        e.h(0xA472, 0, 0, 0, 0, 0, unit, 6, (uint32_t)mx, 0);
        e.wr8(flag, 1);
    }
    for (uint32_t s = 2; s >= 1; s--) {
        const uint32_t node = e.h(0xA5D4, 0, 0, 0, 0, 0, unit, s);
        if (node) { e.h(0xABB6, 0, 0, 0, 0, 0, unit, node); e.h(0xABAA, 0, 0, 0, 0, 0, node); }
    }
    return e.h(0xBC920);
}
}
