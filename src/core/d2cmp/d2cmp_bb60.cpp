// 1.13c D2CMP+0xBB60 -- fastcall, ECX = a node whose +8 is the payload: when the release callback pointer at 0x87F10 is set it is checked with IsBadCodePtr (a bad one reports string 0xB4 through 0x8D90/0x8D8A and exits, 0x28D0) and called with ECX = payload; then payload+0x54 and +0x5C are cleared.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2cmp_87f10;
namespace d2cmp_fn {
void fbb60(GE& e, uint32_t node)
{
    const uint32_t p = e.rd32(node + 8);
    const uint32_t cb = g_d2cmp_87f10;
    if (cb) {
        if (e.h(0x1c158, 0, 0, 0, 0, 0, cb)) {
            const uint32_t s = e.h(0x8d90, 0, 0, 0, 0, 0, 0xb4);
            e.h(0x8d8a, 0, 0, 0, 0, 0, 0xB202d30fu, s);
            e.h(0x28d0, 0, 0, 0, 0, 0, 0xffffffffu);
            return;
        }
        e.h(cb & 0x00ffffffu, 0, p);   // the callee is the slot's own target: its module offset keys the call
    }
    e.wr32(p + 0x54, 0);
    e.wr32(p + 0x5c, 0);
}
}
