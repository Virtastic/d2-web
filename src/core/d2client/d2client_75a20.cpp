// 1.13c D2Client+0x75A20 -- mouse-down on the panel button rows (stack = message, ret 4; Ordinal 511 pops the message).
// g = pinned globals, one dword each: +0 [11BC14] chain head (+0x20 ushort, +0x80 next), +4 [11B9A0] dx, +8 [11B828] x, +0xC [11BFD8] page list head,
// +0x10 [11BBFC] player, +0x14 [11B824] y, +0x18 [11B9A4] dy, +0x1C [11BFF4] (written), +0x20 [11BFFC] (written), +0x24 [DBC48] screen width.
// Pages are linked at +0xCD; each holds five 0x20-byte rows: +0 wide flag, +4 x, +8 y, +0x10 state, +0x14 kind, +0x18 held, +0x1C pressed.
// All compares are signed. The chain at [11BC14] is walked in step with the pages (read only).
// A press with x in [dx+0x116, dx+0x136] and y in [0x185-dy, 0x1A5-dy] sets [11BFF4] = 1, message +0x1C = 0, +0x18 = 1, Ordinal 511. Else x >= width/2
// (truncating) clears message +0x18 and returns. Else rows hit by the 0x14 (narrow) or 0x35 (wide) box with y in [row y - dy - 0x14, row y - dy]:
// a narrow row of kind 1 needs a chain head whose ushort +0x20 is >= 9 (else the walk ends with message +0x18 = 1, +0x1C = 0, Ordinal 511) and
// Ordinal 10331(player) and Ordinal 10057(result) both nonzero (else the row is skipped). A narrow row with +0x1C == 0 gets +0x10 += 1, +0x1C = 1;
// a wide row sets +0x10 = 1 unless +0x18 == 1, and +0x1C = 1. Either sets [11BFFC] = 1 and message +0x18 = 1, +0x1C = 0, Ordinal 511.
#include "cl_env.h"
extern "C" void d2c_75a20(uint32_t msg, const cl::Env& e, uint32_t g) {
    uint32_t chain = e.R(g) ? e.R(e.R(g) + 0x80) : 0;
    int32_t dx = (int32_t)e.R(g + 4), x = (int32_t)e.R(g + 8), y = (int32_t)e.R(g + 0x14), dy = (int32_t)e.R(g + 0x18);
    uint32_t page = e.R(g + 0xc), pl = e.R(g + 0x10);
    if (!(x < dx + 0x116 || x > dx + 0x136 || y < 0x185 - dy || y > 0x1a5 - dy)) {
        e.W(msg + 0x1c, 0); e.W(g + 0x1c, 1); e.W(msg + 0x18, 1); e.H(0xbe84, msg); return;
    }
    int32_t w = (int32_t)e.R(g + 0x24); w = (w - (w >> 31)) >> 1;
    if (x >= w) { e.W(msg + 0x18, 0); return; }
    while (page != 0) {
        for (int32_t i = 0; i < 5; i++) {
            uint32_t b = page + (uint32_t)i * 0x20;
            int32_t bx = (int32_t)e.R(b + 4) + dx, by = (int32_t)e.R(b + 8) - dy;
            if (e.R(b) == 0) {
                if (x < bx || x > bx + 0x14 || y < by - 0x14 || y > by) continue;
                if (e.R(b + 0x14) == 1) {
                    uint32_t c = e.R(g);
                    if (c == 0 || (int32_t)(e.R(c + 0x20) & 0xffff) < 9) goto done;
                    uint32_t r = e.H(0xc1f0, pl);
                    if (r == 0 || e.H(0xc208, r) == 0) continue;
                }
                if (e.R(b + 0x1c) != 0) continue;
                e.W(b + 0x10, e.R(b + 0x10) + 1); e.W(b + 0x1c, 1); e.W(g + 0x20, 1); e.W(msg + 0x18, 1);
                goto fin;
            } else {
                if (x < bx || x > bx + 0x35 || y < by - 0x14 || y > by) continue;
                if (e.R(b + 0x18) != 1) e.W(b + 0x10, 1);
                e.W(b + 0x1c, 1); e.W(g + 0x20, 1); e.W(msg + 0x18, 1);
                goto fin;
            }
        }
        page = e.R(page + 0xcd);
        if (chain != 0) chain = e.R(chain + 0x80);
    }
done:
    e.W(msg + 0x18, 1);
fin:
    e.W(msg + 0x1c, 0);
    e.H(0xbe84, msg);
}
