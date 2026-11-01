// 1.13c D2Client+0x26970: life/mana orb hint text (mouse over the bar) and the 2-line fill bar under it.
#include "d2client_callee.h"
// 1.13c D2Client+0x26970
extern "C" void d2c_26970(const D2cSvc& v, const uint32_t* unit /*[0x11BBFC]*/, int32_t w, int32_t h, int32_t mouseA /*0x11B828*/, int32_t mouseB /*0x11B824*/) {
    const uint32_t u = (uint32_t)(uintptr_t)unit;
    const uint32_t type = unit ? unit[1] : 0xffffffffu;
    const uint32_t cur = d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, u, 0xc, 0);            // D2Common #10973 (unit, 0xc, 0)
    const uint32_t mx = d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, u, 0xd, 0);             // (unit, 0xd, 0)
    const uint32_t q = d2c_call(v, 0xc250, 0, 0, 0, 0, 0, u, 0xc, 0);              // D2Common #10587 (unit, 0xc, 0)
    const uint32_t total0 = d2c_call(v, 0xc24a, 0, 0, 0, 0, 0, type, q);           // D2Common #10399 (type, q)
    const uint32_t base = d2c_call(v, 0xc24a, 0, 0, 0, 0, 0, type, cur - 1);       // (type, cur-1)
    const uint32_t total = total0 - base;
    const uint32_t have = mx > base ? mx - base : 0;
    const int32_t w2 = (w - (w >> 31)) >> 1;
    if (mouseA >= w2 - 0x92 && mouseA <= w2 - 0x17 && mouseB >= h - 0x2b && mouseB <= h - 0x22) {
        uint32_t buf[50] = {};   // 100 UTF-16 cells; Unicode::Unicode (import, pinned) clears each, Unicode::sprintf (import, pinned) fills it
        const uint32_t s = d2c_call(v, 0xd1ec, 0, 0x1043, 0, 0, 0);                // D2Lang #10003 (ECX = string id); total0 and mx stay pushed for sprintf
        (void)s;
        d2c_call(v, 0xd384, 0, (uint32_t)(uintptr_t)buf, (uint32_t)(w2 - 0x92), 0, 0, (uint32_t)(h - 0x33), 0, 1);   // D2Win #10085 (ECX text, EDX x, stack y 0 1)
    }
    if (total == 0) return;
    uint32_t num = have, den = total;
    if (num >= 0x226b902u) { num >>= 7; den >>= 7; }
    uint32_t bar = (num * 0x77u) / den;   // unsigned div; den == 0 after the shift would trap in retail too
    if (bar > 0x77) bar = 0;
    const uint32_t lim = d2c_call(v, 0xc28c, 0, 0, 0, 0, 0, type);                 // D2Common #10066 (type)
    if (cur >= lim || bar == 0) return;
    const uint32_t x0 = (uint32_t)(w2 - 0x90), y = (uint32_t)(h - 0x26);
    d2c_call(v, 0xd102, 0, 0, 0, 0, 0, x0, y, x0 + bar, y, 0xff, 0xff);           // D2Gfx #10010 (x0, y, x1, y, 0xff, 0xff)
    d2c_call(v, 0xd102, 0, 0, 0, 0, 0, x0, y + 1, x0 + bar, y + 1, 0xff, 0xff);
}
