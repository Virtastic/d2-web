// 1.13c D2Game+0x76A50 -- ECX = game, EDX = unit; no result. `mask` is the dword behind D2Game's IAT data slot 0xF8BE0 and
// `fp` the module callback global 0xFBC1C (0, or 0xB2000000|RVA under the FN pins). Written from the 1.13c disassembly.
// 0xA496(unit, 0). The 8-dword request {8, unit, o, 0, 0, byte +0x15 = 0x64 (0x65 when 0x1320(EAX class) gives a row whose
// byte +0x105 & mask is set), 0, 0} with o = 0xA598(unit+0x2C) (0 when it is the unit itself) goes to 0x75E10(game, &req, 1)
// after 0xE07A0(EAX unit, ECX game). Then unless 0xA68E(unit, 1) is set and 0xDFA90(EAX unit) clear, a callback
// fp(ECX game, EDX unit; 0, 0).
#include "../fn/hookenv.h"
namespace d2g {
void f76a50(HookEnv& e, uint32_t game, uint32_t unit, uint32_t mask, uint32_t fp)
{
    e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
    uint32_t req[8] = { 8, unit, 0, 0, 0, 0, 0, 0 };
    uint32_t bl = 0x64;
    if (unit) {
        const uint32_t m = e.h(0x1320, e.rd32(unit + 4));
        if (m && (e.rd8(m + 0x105) & mask)) bl = 0x65;
    }
    req[5] = bl << 8;
    e.h(0xE07A0, unit, game);
    const uint32_t o = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    req[2] = o == unit ? 0 : o;
    e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(req), 1);
    e.blk(0x75E10, req, 8);
    if (e.h(0xA68E, 0, 0, 0, 0, 0, unit, 1) && !e.h(0xDFA90, unit)) return;
    if (fp) e.h(fp & 0xFFFFFF, 0, game, unit, 0, 0, 0, 0);
}
}
