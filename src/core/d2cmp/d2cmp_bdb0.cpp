// 1.13c D2CMP+0xBDB0: release idle cel resources. cdecl().
#include "d2cmp_cache32.h"
using namespace d2cmp32;
// head = [6FE7725C]; list node: +00 cel, +14 next; cel +B0 resource.
// 1.13c D2CMP+0xBDB0
extern "C" void d2cmp_bdb0(uint32_t head, const Env& e) {
    for (uint32_t n = head; n; n = ld(n + 0x14)) {
        uint32_t cel = ld(n);
        if (!cel) continue;
        uint32_t res = ld(cel + 0xB0);
        if (!res) continue;
        if (e.celBusy(e.ctx, res) == 0) {   // Ordinal 10092
            e.celRelease(e.ctx, res);        // Ordinal 10097
            st(cel + 0xB0, 0);
        }
    }
}
