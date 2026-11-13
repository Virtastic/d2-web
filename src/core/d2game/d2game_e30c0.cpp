// 1.13c D2Game+0xE30C0 -- ECX = game (byte +0x6D selects the flag buffer), EDX = unit, one ignored stack word (ret 4). Gets the player data
// through the D2Common import 0xA4C0(unit) and the buffer pd[0x10 + 4*difficulty]. If 0xA568(buf, 0x20, 0) is zero and 0xA478(unit) is non-zero,
// 0xA556(0xA478(unit)) non-zero and 0xA56E(that) has AL == 2, the import 0xA57A(buf, 0x20, 0) clears the bit. Returns 0.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fe30c0(HookEnv& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x20, 0) != 0) return 0;
    if (!e.h(0xA478, 0, 0, 0, 0, 0, unit)) return 0;
    const uint32_t a = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    const uint32_t b = e.h(0xA556, 0, 0, 0, 0, 0, a);
    if (!b) return 0;
    if ((e.h(0xA56E, 0, 0, 0, 0, 0, b) & 0xFF) == 2) e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0x20, 0);
    return 0;
}
}
