// 1.13c D2Client+0x29B50 -- belt right-button-down listener (message in [esp+4], ret 4). The four globals are passed by address:
// gLeave = leaving flag [0x11C3A4], gPlayer = player unit [0x11BBFC], gMode = cursor mode [0x11B864], gHover = belt hover flag [0x11C468].
// Nothing is taken (msg+0x18 = 0) when leaving, no player, or the player's mode (+0x10) is 0x11 (dead). Outside the belt band
// (0x26320) the message stays unhandled and the hover flag clears. Inside: handled (+0x18 = 1, +0x1C = 0); cursor mode 6 calls 0xBEB90,
// mode 8 calls 0x14940 (EAX 0x4C, ECX -1); box = 0x26460(y = msg+0xE, EDI = x = msg+0xC); a negative box leaves it unhandled; else
// useBelt 0x29990 (EAX = (msg+8 >> 2) & 1, ECX = player+0x60, EDX = player, slot = box) and the message goes on (0xBE84).
#include "cl_env.h"
extern "C" void d2c_29b50(uint32_t msg, const cl::Env& e, uint32_t gLeave, uint32_t gPlayer, uint32_t gMode, uint32_t gHover) {
    uint32_t pl = e.R(gPlayer);
    if (e.R(gLeave) != 0 || pl == 0 || e.R(pl + 0x10) == 0x11) { e.W(msg + 0x18, 0); return; }
    uint32_t w = e.R(msg + 0xc);
    uint32_t x = w & 0xffff, y = e.R(msg + 0xc) >> 16;
    if (e.H(0x26320, 0, 0, 0, 0, 0, msg) == 0) { e.W(msg + 0x18, 0); e.W(gHover, 0); return; }
    uint32_t mode = e.R(gMode);
    e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0);
    if (mode == 6) e.H(0xbeb90);
    else if (mode == 8) e.H(0x14940, 0, 0, 0, 0, 0, 0x4c, 0xffffffffu);
    int32_t box = (int32_t)e.H(0x26460, y);
    e.Ej(0x26460, x);
    if (box < 0) { e.W(msg + 0x18, 0); return; }
    e.H(0x29990, (uint32_t)box, 0, 0, 0, 0, (e.R(msg + 8) >> 2) & 1, e.R(pl + 0x60), pl);
    e.H(0xbe84, msg);
}
