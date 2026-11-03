// d2client+0x27a90 probe: r[0] = st (W H cellA cellB, then the scripted hook results), r[1] = hooklog, r[2] = the player unit blob.
#include <emscripten.h>
#include "d2client_fnlog.h"
extern "C" void d2c_27a90(const D2cSvc&, uint32_t, int32_t, int32_t, uint32_t, uint32_t);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_27a90(uint32_t* r) {
    const uint32_t* st = (const uint32_t*)(uintptr_t)r[0];   // 4 cur, 5 max, 6 c1fc#1, 7 c1fc#2, 8 c1c6, 9 d10e#1, 10 d10e#2
    FnHk t[] = { { 0x26b20, 0, "a", -1, 0, 0, 0, st[4] }, { 0xcf4c, 1, "-", -1, 0, 0, 0, st[5] },
                 { 0xc1fc, 2, "-", -1, 0, 0, 0, st[6] }, { 0xc1c6, 3, "-", -1, 0, 0, 0, st[8] }, { 0xd10e, 0, "-", -1, 0, 0, 0, st[9] },
                 { 0xd1b6, 6, "h", 0, 18, 0xFFFFFFFFu, 1, 0 }, { 0xd096, 6, "h", 0, 18, 0xFFFFFFFFu, 1, 0 } };
    t[2].seq[0] = st[6]; t[2].seq[1] = st[7]; t[2].nseq = 2;
    t[4].seq[0] = st[9]; t[4].seq[1] = st[10]; t[4].nseq = 2;
    FnLog L = { (uint32_t*)(uintptr_t)r[1], t, 7 }; L.lg[0] = 0;
    d2c_27a90({ &L, fnlog_call }, r[2], (int32_t)st[0], (int32_t)st[1], st[2], st[3]);
    r[4] = 0;
}
