// 1.13c D2Client+0x37C80 (cdecl, no arguments): the head of the frame-step routine, 0x37C80..0x37DB4. State block `st` (dwords):
// 0 [0xFADD4] done flag, 1 [0xDD5D0] panel count, 2 [0x113A68], 3 [0x11C44C], 5 [0x11971D], 6 [0x119721], 7 [0x11BBFC] local unit,
// 0x10.. [0xFAD80] 0x26 active flags, 0x36.. [0xDD538] 0x26 panel pointers (mode at +0x54). Callees through the injected dispatcher.
#include <cstdint>
#include <cstring>
#include "d2client_callee.h"
static inline uint32_t rd32_37c80(uint32_t a) { uint32_t x; std::memcpy(&x, (const void*)(uintptr_t)a, 4); return x; }
// 1.13c D2Client+0x37C80
extern "C" void d2c_37c80(const D2cSvc& v, uint32_t* st) {
    const uint32_t* act = st + 0x10, *pt = st + 0x36;
    if (!st[0]) {
        d2c_call(v, 0xbf8c, 0, 0, 0, 0, 0);                                        // Ordinal_10227()
        if (st[1] != 0x26) {
            const uint32_t m = d2c_call(v, 0xbf92, 0, 0, 0, 0, 0, 0x139b);
            d2c_call(v, 0xbf80, 0, 0, 0, 0, 0, 0xB2000000u | 0xD359Fu, m);
            d2c_call(v, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
        }
        if (!st[2]) {
            uint32_t i = 0; bool leave = false;
            do {
                if (act[i] == 1) {
                    const uint32_t mode = rd32_37c80(pt[i] + 0x54);
                    if (mode > 4) { leave = true; break; }
                    if (mode == 1) d2c_call(v, 0xc2790, 0, i, 1, 0, 0, 0);
                    else if (mode == 2 || mode == 3) { leave = true; break; }
                    else if (mode == 4 && st[6]) d2c_call(v, 0x47b20, 0, 0, 0, 0, 0);   // EDI = [0x11971D]
                }
                ++i;
            } while ((int32_t)i < (int32_t)st[1]);
            if (!leave) {
                const uint32_t u = st[7]; bool speak;
                if (!u) { st[0] = 1; speak = true; }
                else if (rd32_37c80(u) != 0) speak = st[0] != 0;
                else if (d2c_call(v, 0xa5440, u, 0, 0, 0, 0)) speak = st[0] != 0;
                else { st[0] = 1; speak = true; }
                if (speak) {
                    const uint32_t s = d2c_call(v, 0xd0b4, 0, 0, 0, 0, 0, 0xB2000000u | 0xD0864u);
                    d2c_call(v, 0x3ccc0, 2, 0, 0, 0, 0, s);
                }
            }
        }
        st[3] = 1;
    }
    if (d2c_call(v, 0xd0ae, 0, 0, 0, 0, 0) == 2) {
        d2c_call(v, 0x361d0, 0, 0, 0, 0, 0); d2c_call(v, 0x36660, 0, 0, 0, 0, 0);
        d2c_call(v, 0x36170, 0, 0, 0, 0, 0); d2c_call(v, 0x359b0, 0, 0, 0, 0, 0); d2c_call(v, 0x36ce0, 0, 0, 0, 0, 0);
    } else {
        d2c_call(v, 0x362e0, 0, 0, 0, 0, 0); d2c_call(v, 0x36660, 0, 0, 0, 0, 0);
        d2c_call(v, 0x36170, 0, 0, 0, 0, 0); d2c_call(v, 0x35df0, 0, 0, 0, 0, 0); d2c_call(v, 0x36710, 0, 0, 0, 0, 0);
    }
}
