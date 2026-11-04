// d2client+0x278b0 probe: r[0] = st (W H mouseX mouseY pal, then the scripted results), r[1] = hooklog, r[2] = the player unit blob.
#include <emscripten.h>
#include "d2client_fnlog.h"
#include <cstring>
extern "C" void d2c_278b0(const D2cSvc&, uint32_t, int32_t, int32_t, int32_t, int32_t, uint8_t*);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_278b0(uint32_t* r) {
    uint32_t* st = (uint32_t*)(uintptr_t)r[0];   // 5 cur1, 6 max1, 7 cur2, 8 max2, 9 cbda, 10 c1fc, 11-13 d390 x3, 14 d1ec
    FnHk t[] = { { 0x26b20, 0, "a", -1, 0, 0, 0, 0 }, { 0xcf34, 1, "-", -1, 0, 0, 0, 0 }, { 0xcbda, 2, "-", -1, 0, 0, 0, st[9] },
                 { 0xc1fc, 2, "-", -1, 0, 0, 0, st[10] }, { 0xd390, 3, "-", -1, 0, 0, 0, 0 }, { 0xd0ba, 6, "h", -1, 0, 0, 0, 0 },
                 { 0xd1ec, 2, "ck", -1, 0, 0, 0, st[14] }, { 0xd384, 3, "cd", -1, 0, 0, 0, 0, 2 } };
    t[0].seq[0] = st[5]; t[0].seq[1] = st[7]; t[0].nseq = 2;
    t[1].seq[0] = st[6]; t[1].seq[1] = st[8]; t[1].nseq = 2;
    t[4].seq[0] = st[11]; t[4].seq[1] = st[12]; t[4].seq[2] = st[13]; t[4].nseq = 3;
    uint8_t pal[4]; std::memcpy(pal, &st[4], 4);   // the pinned global is not read back by the capture
    FnLog L = { (uint32_t*)(uintptr_t)r[1], t, 8 }; L.lg[0] = 0;
    d2c_278b0({ &L, fnlog_call }, r[2], (int32_t)st[0], (int32_t)st[1], (int32_t)st[2], (int32_t)st[3], pal);
    r[4] = 0;
}
