// d2client+0x27ef0 probe: r[0] = st (W H, e9f30 results), r[1] = hooklog, r[2] = the key table blob (0x11A2E0, 1152 bytes).
#include <emscripten.h>
#include "d2client_fnlog.h"
extern "C" void d2c_27ef0(const D2cSvc&, int32_t, int32_t, const uint8_t*);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_27ef0(uint32_t* r) {
    const uint32_t* st = (const uint32_t*)(uintptr_t)r[0];
    FnHk t[] = { { 0x39f30, 0, "ad", -1, 0, 0, 0, 0 }, { 0xd1ec, 0, "c", -1, 0, 0, 0, 0 }, { 0xd384, 3, "cd", -1, 0, 0, 0, 0, 2 } };
    t[0].seq[0] = st[2]; t[0].seq[1] = st[3]; t[0].nseq = 2;
    FnLog L = { (uint32_t*)(uintptr_t)r[1], t, 3 }; L.lg[0] = 0;
    d2c_27ef0({ &L, fnlog_call }, (int32_t)st[0], (int32_t)st[1], (const uint8_t*)(uintptr_t)r[2]);
    r[4] = 0;
}
