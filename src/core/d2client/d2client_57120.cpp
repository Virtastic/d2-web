// 1.13c D2Client+0x57120 -- copy a control's two values onto a panel widget (ECX = panel, stack word unused, ret 4).
// Panel +0x2C = control record (null: retail's fatal path, never reached by the vector). Ordinal 10392(handle) = record pp (zero: return 0),
// Ordinal 10511(panel) = widget b (zero: return 0). Record type pp[0]: "direct" when 2 or 4..5 (signed). Value 1 = direct ? pp[0xB][3] : (pp[0xB] ? 10110ish
// lookup 0xC3BE(pp[0xB]) : 0); 10243(b, value1). Value 2 = direct ? pp[0xB][4] : (pp[0xB] ? 0xC2EC(pp[0xB]) : 0); 10554(b, value2). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_57120(uint32_t p, const cl::Env& e) {
    e.H(0x2a0b0, 0, 0, 0, 0, 0, p);
    uint32_t f = e.R(p + 0x2c);
    if (f == 0) { uint32_t m = e.H(0xbf92, 0x946); e.H(0xbf80, 0xB2000000u | 0xD359F, m); e.H(0x3664, 0xffffffffu); return 0; }
    uint32_t pp = e.H(0xc2da, f);
    if (pp == 0) return 0;
    uint32_t b = e.H(0xc310, p);
    if (b == 0) return 0;
    int32_t t = (int32_t)e.R(pp);
    bool direct = t == 2 || (t > 3 && t <= 5);
    uint32_t sub = e.R(pp + 0x2c), v;
    if (direct) v = e.R(sub + 0xc); else v = sub ? e.H(0xc3be, sub) : 0;
    e.H(0xcb50, b, v);
    t = (int32_t)e.R(pp);
    direct = t == 2 || (t > 3 && t <= 5);
    sub = e.R(pp + 0x2c);
    if (direct) { e.H(0xcb7a, b, e.R(sub + 0x10)); return 1; }
    e.H(0xcb7a, b, sub ? e.H(0xc2ec, sub) : 0);
    return 1;
}
