// 1.13c D2Launch+0x19CE0 -- fastcall, ECX = the launch-state struct (stored at 0x25ABC): the main-menu driver. D2Win/D2Gfx ordinals 10100, 10227, 10016/10008 bring the screen up (a failure reports string 0x1241 and exits(-1)), the four flag bytes (+0x35D +0x37D +0x1B +0x241, and +0x33 unless the screen index 0x25AD4 is 2) are cleared,
// ordinal 10048 registers the 11 (15 with 10227 set) control resources, [param+0x221] is kept at 0x256C4 and, by the screen byte +0x35A: 2 / 4 call slot +0x68 of that table (non-zero: 0xD5D0, zero: 0x19290; screen index 1 / 3), 3 runs 0xE320 + 0x17B30 (index 2), else 0xE320 and by +0x359 / +0x35B the "Skip To Open" registry value (Storm 423), 0x19290, 0x19700 or 0x17DF0.
// Then the two handlers (0x121A0, 0x11FA0), 10019, the 10036 poll loop with Sleep(0), 10017, 0x15D30, 0x12C80, 10005, the six dwords at +0xA1 are zeroed, 10144; returns [0x25AD0].
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25abc, g_d2launch_25c08, g_d2launch_25ad4, g_d2launch_256c4, g_d2launch_25ad0;
namespace d2launch {
uint32_t f19ce0(GE& e, uint32_t p)
{
    g_d2launch_25abc = p;
    e.h(0x986e);
    const uint32_t a = e.h(0x980e);
    const uint32_t b = e.h(0x98e0, 0, 0, 0, 0, 0, a);
    if (!e.h(0x9b1a, 0, b, 0)) {
        const uint32_t s = e.h(0x9820, 0, 0, 0, 0, 0, 0x1241);
        e.h(0x97f6, 0, 0, 0, 0, 0, 0xB201e256u, s);
        e.h(0x165b, 0, 0, 0, 0, 0, 0xffffffffu);
        return 0;
    }
    e.h(0x13010);
    e.h(0x9886, 0, 0, 0, 0, 0, g_d2launch_25c08);
    const uint32_t q = g_d2launch_25abc, idx0 = g_d2launch_25ad4;
    e.wr8(q + 0x35d, 0); e.wr8(q + 0x37d, 0); e.wr8(q + 0x1b, 0); e.wr8(q + 0x241, 0);
    if (idx0 != 2) e.wr8(q + 0x33, 0);
    e.h(0x9a6c);
    static const uint32_t k1[11] = { 0x1dd78, 0x1decc, 0x1e034, 0x1de98, 0x1e000, 0x1de68, 0x1dfcc, 0x1de34, 0x1df94, 0x1de00, 0x1df60 };
    static const uint32_t k2[4] = { 0x1dd9c, 0x1df30, 0x1ddcc, 0x1defc };
    for (int i = 0; i < 11; i++) e.h(0x9a78, 0, 0xB2000000u | k1[i]);
    if (e.h(0x980e)) for (int i = 0; i < 4; i++) e.h(0x9a78, 0, 0xB2000000u | k2[i]);
    const uint32_t vt = e.rd32(p + 0x221);
    g_d2launch_256c4 = vt;
    const uint32_t scr = e.rd8(g_d2launch_25abc + 0x35a);
    if (scr == 2 || scr == 4) {
        const uint32_t r = e.h(0x9b32, 0, vt);   // call [vt + 0x68]
        g_d2launch_25ad4 = scr == 2 ? 1 : 3;
        e.h(0x9898, 0, 0, 0, 0, 0, 1);
        e.h(r ? 0xd5d0 : 0x19290);
    } else if (scr == 3) {
        e.h(0xe320);
        g_d2launch_25ad4 = 2;
        e.h(0x9898, 0, 0, 0, 0, 0, 1);
        e.h(0x17b30);
    } else {
        e.h(0xe320);
        const uint32_t g = g_d2launch_25abc;
        if (e.rd8(g + 0x359)) {
            uint32_t out = 0;
            e.h(0x976c, 0, 0, 0, 0, 0, 0xB201e204u, 0xB201dd68u, 0, e.stk(&out));
            e.wr8(g + 0x359, 0); e.wr8(g + 0x35b, 1);
            e.h(0x9898, 0, 0, 0, 0, 0, 1);
            e.h(0x9b38, 0, 0, 0, 0, 0, 1);
            g_d2launch_25ad4 = (out != 0) * 2 + 1;
            e.h(0x19290);
        } else {
            const bool seen = e.rd8(g + 0x35b) != 0;
            g_d2launch_25ad4 = 0;
            if (!seen) {
                e.h(0x9898, 0, 0, 0, 0, 0, 1);
                e.h(0x9b38, 0, 0, 0, 0, 0, 1);
                e.h(0x19700);
                e.wr8(g_d2launch_25abc + 0x35b, 1);
            } else {
                e.h(0x9898, 0, 0, 0, 0, 0, 1);
                e.h(0x17df0);
            }
        }
    }
    e.h(0x98a4, 0, 0, 0, 0, 0, 0xB20121a0u);
    e.h(0x99a0, 0, 0, 0, 0, 0, 0xB2011fa0u);
    e.h(0x9a5a);
    for (uint32_t v = e.h(0x9a42); v; v = e.h(0x9a42)) e.h(0x1c360, 0, 0, 0, 0, 0, 0);   // Sleep(0)
    e.h(0x9a66);
    e.h(0x15d30);
    e.h(0x12c80);
    e.h(0x9b20);
    const uint32_t w = g_d2launch_25abc;
    for (uint32_t o = 0xa1; o <= 0xb5; o += 4) e.wr32(w + o, 0);
    e.h(0x98f2);
    return g_d2launch_25ad0;
}
}
