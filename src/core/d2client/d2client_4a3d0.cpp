// 1.13c D2Client+0x4A3D0 -- EAX = value compared with [11BBFC]; nothing happens unless equal. Then every record (0x16 bytes from F37A8, count [F3B9C]) whose
// flag byte (+0x11) is 1 gets it cleared and 46910 called (ESI = the flag byte's address), and byte +0x15 set to 1 for every record. Then, when 46150() is non-zero:
// [119663] = 0, the six dwords at E0EA8 = -1, [11BC28]/[11BC2C]/[11BC38]/[11BC3C] = 0, 47B20(), BF8C(); [FADA0] = 0, and when it was non-zero BE2E0() and 492F0();
// [119721] = 0. The two fatal paths (a flagged record with [11BBFC] = 0; [DD5D0] != 0x26) go through the host and are not captured.
// st: g11bbfc, gF3B9C, g119663, E0EA8 x6, g11bc28, g11bc2c, g11bc38, g11bc3c, gFADA0, gDD5D0, g119721 (dwords). tb = the data at F37A8.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_hcall.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_4a3d0(D2cHost h, int32_t eax, uint32_t* st, uint8_t* tb) {
    if ((uint32_t)eax != st[0]) return;
    int32_t n = (int32_t)st[1];
    for (int32_t i = 0; i < n; ++i) {
        uint8_t* f = tb + 0x11 + 0x16 * i;
        if (f[0] == 1) {
            f[0] = 0;
            if (st[0] == 0) { d2cCall(h, 0xBF92, 0xFAB); return; }
            d2cCall(h, 0x46910, 0, 0, 0, 0, 0, 0, 0, 0, 0xB2000000u | (0xF37B9 + 0x16 * (uint32_t)i));
        }
        f[4] = 1;
    }
    if (!d2cCall(h, 0x46150)) return;
    st[2] = 0;
    for (int k = 0; k < 6; ++k) st[3 + k] = 0xFFFFFFFFu;
    st[9] = st[10] = st[11] = st[12] = 0;
    d2cCall(h, 0x47B20);
    uint32_t prev = st[13];
    d2cCall(h, 0xBF8C);
    if (st[14] != 0x26) { d2cCall(h, 0xBF92, 0x139B); return; }
    st[13] = 0;
    if (prev) { d2cCall(h, 0xBE2E0); d2cCall(h, 0x492F0); }
    st[15] = 0;
}
