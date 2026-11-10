// 1.13c D2Client+0xA3E40 (497 B, 11 stack args a0..a10, ret 0x2C): open a dialog node. With 0xFADA8 set, log 0x80780 through
// 0x3CC60 (EAX = 5, stack 0xD0B4(), text). Cursor state reset: 0xE0EA8..0xE0EBC = -1, 0xF46C4/C8 = 0x10, 0xE0EA0, 0x11BC28/2C/38/3C,
// 0x11C3DC/E0/F8/FC, 0x11C2F8/F4 = 0. a2 == 0 returns 0. Else 0x11BC08 += 1; 0xBF7A (ECX = 0xB10, EDX = file, stack 0x3ED, 0)
// allocates the node, cleared twice (the old head stored at +0xB0C between the passes is wiped); the node becomes head 0x11BC00
// and takes the arguments, GetTickCount at +0, -1 at +0x44/+0x48, 2 at +0x5C. 0x11BC04 = 1; 0xA3A20 (EAX = a10, ECX = a8,
// EDX = a4, EBX = 1, ESI = node); log 0x8E858 via 0x3CCC0 (EAX = 0xE) and, when 0xF2090 is nonzero, 0x8E900 (EAX = 1);
// 0x10B208 += 1; a count other than 1 is fatal (0xBF92(0x410), 0xBF80, _exit(-1)). Returns the node.
#include <initializer_list>
#include "cl_env.h"
extern "C" uint32_t d2c_a3e40(const uint32_t* a, uint32_t now, const cl::Env& e) {
    constexpr uint32_t B = 0x6FAB0000u, M = 0xB2000000u;
    if (e.R(B + 0xFADA8)) { const uint32_t v = e.H(0xd0b4); e.H(0x3cc60, v, M + 0xD0780, 0, 0, 0, 5); }
    for (uint32_t o = 0; o < 0x18; o += 4) e.W(B + 0xE0EA8 + o, 0xffffffffu);
    e.W(B + 0xF46C8, 0x10); e.W(B + 0xF46C4, 0x10);
    for (uint32_t r : { 0xE0EA0u, 0x11BC28u, 0x11BC2Cu, 0x11BC38u, 0x11BC3Cu, 0x11C3DCu, 0x11C3E0u, 0x11C3F8u, 0x11C3FCu, 0x11C2F8u, 0x11C2F4u })
        e.W(B + r, 0);
    if (!a[2]) return 0;
    e.W(B + 0x11BC08, e.R(B + 0x11BC08) + 1);
    const uint32_t n = e.H(0xbf7a, 0x3ed, 0, 0, 0, 0, 0, 0xb10, M + 0xD41D4);
    for (uint32_t o = 0; o < 0xb10; o += 4) e.W(n + o, 0);
    e.W(n + 0xb0c, e.R(B + 0x11BC00));
    for (uint32_t o = 0; o < 0xb10; o += 4) e.W(n + o, 0);
    e.W(n + 0x24, a[0]); e.W(n + 0x2c, a[0]); e.W(n + 0x60, a[2]); e.W(n + 0x64, a[3]); e.W(n + 0x58, a[6]);
    e.W(B + 0x11BC00, n);
    e.W(n + 0x38, 0); e.W(n + 0x50, 0); e.W(n + 0x4c, 0); e.W(n + 0x44, 0xffffffffu); e.W(n + 0x5c, 2); e.W(n + 0xb08, 0);
    e.W(n + 0x28, a[1]); e.W(n + 0x30, a[1]); e.W(n + 0x14, a[5]); e.W(n + 0x1c, a[9]); e.W(n + 0x34, a[7]);
    e.W(n, now);
    e.W(n + 0x10, 0); e.W(n + 0x54, 0); e.W(n + 0xc, 0); e.W(n + 8, 0);
    e.W(n + 0x38, a[8]); e.W(n + 0x48, 0xffffffffu); e.W(n + 0x18, a[4]); e.W(n + 0x20, a[10]);
    e.W(B + 0x11BC04, 1);
    e.H(0xa3a20, 0, 0, 0, 0, 0, a[10], a[8], a[4], 1, n);
    { const uint32_t v = e.H(0xd0b4); e.H(0x3ccc0, v, M + 0xDE858, 0, 0, 0, 0xe); }
    if (e.H(0x42090)) { const uint32_t v = e.H(0xd0b4); e.H(0x3ccc0, v, M + 0xDE900, 0, 0, 0, 1); }
    e.W(B + 0x10B208, e.R(B + 0x10B208) + 1);
    if (e.R(B + 0x11BC08) != 1) {   // retail does not return from _exit
        const uint32_t msg = e.H(0xbf92, 0x410);
        e.H(0xbf80, M + 0xD359F, msg);
        e.H(0x3664, 0xffffffffu);
    }
    return n;
}
