// 1.13c D2Client+0x7CFC0 (529 B, EAX = output Unicode buffer, one stack arg = message record, ret 4): chat/event line formatter.
// id = dword at msg+3 (unaligned), kind = byte msg+7. 0x9D630(EDX = id) is the player record; its name (+0x66, ASCII) is widened with
// win2Unicode (0xCEAC0, ECX = 16-wchar local, EDX = name, 0x10) into a Unicode local. Kinds 1,2,10,11 sprintf (0xCEAB4) the string
// 0x277A/0x277B/0x277C/0x277D with the name; 3,4 strcpy name + strcat 0xFBA/0xFBB (3 also 0x88A70 and returns 1); 5,6,7,9 name +
// 0xFBC/0xFBD/0xFBE/0xFC0; 8 string 0xFBF + name. After that, if playerSelf (0x11BBFC) is set: 0x4FD90(EDX = self[+0xC], ESI =
// player[+0x10] or -1, 4) non-zero returns 0; otherwise 1.
#include "cl_env.h"
// 1.13c D2Client+0x7CFC0
extern "C" uint32_t d2c_7cfc0(uint32_t out, uint32_t msg, uint32_t ps, const cl::Env& e) {
    const auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    const uint32_t id = e.R(msg + 3);
    const uint32_t pl = e.H(0x9d630, 0, 0, 0, 0, 0, 0, 0, id);
    uint16_t L[16];
    for (uint32_t i = 0; i < 16; ++i) L[i] = 0;   // Unicode default ctor 0xCEB00 (zeroes a wchar; not a recorded call)
    const uint32_t self = e.R(ps);
    char nm[16];
    if (pl) { uint32_t i = 0; char c; do { c = (char)e.R8(pl + 0x66 + i); if (i < 16) nm[i] = c; ++i; } while (c); }
    else nm[0] = 0;
    e.H(0xceac0, 0x10, 0, 0, 0, 0, 0, P(L), P(nm));
    const auto cpy = [&](uint32_t d, uint32_t s) { e.H(0xceab8, 0, 0, 0, 0, 0, 0, d, s); };
    const auto cat = [&](uint32_t d, uint32_t s) { e.H(0xceabc, 0, 0, 0, 0, 0, 0, d, s); };
    const auto str = [&](uint32_t k) { return e.H(0xd1ec, 0, 0, 0, 0, 0, 0, k); };
    const uint32_t kind = e.R8(msg + 7);
    switch (kind) {
    case 1: case 2: case 10: case 11: {
        const uint32_t f = str(kind == 1 ? 0x277a : kind == 2 ? 0x277b : kind == 10 ? 0x277c : 0x277d);
        e.H(0xceab4, 0x132, out, f, P(L), 0, 0);
        break;
    }
    case 3: cpy(out, P(L)); cat(out, str(0xfba)); e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 0x10, 0); return 1;
    case 4: cpy(out, P(L)); cat(out, str(0xfbb)); return 1;
    case 5: cpy(out, P(L)); cat(out, str(0xfbc)); break;
    case 6: cpy(out, P(L)); cat(out, str(0xfbd)); break;
    case 7: cpy(out, P(L)); cat(out, str(0xfbe)); break;
    case 8: { const uint32_t s = str(0xfbf); cpy(out, s); cat(out, P(L)); break; }
    case 9: cpy(out, P(L)); cat(out, str(0xfc0)); break;
    default: break;
    }
    if (!self) return 1;
    const uint32_t esi = pl ? e.R(pl + 0x10) : 0xffffffffu;
    return e.H(0x4fd90, 4, 0, 0, 0, 0, 0, 0, e.R(self + 0xc), 0, esi) ? 0 : 1;
}
