// d2client+0x289a0 probe: r[0] = st (enable g46c g470 g538c hx hy, then 6 c202, 7 c1ba, 8 c19c, 9 3b760, 10 46230 result, 11 dword 0x46230 writes at its out buffer,
// 12-13 6f5f0 results), r[1] = hooklog, r[2] = the player unit blob, r[3] = the item blob.
#include <emscripten.h>
#include "d2client_fnlog.h"
extern "C" void d2c_289a0(const D2cSvc&, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, int32_t, int32_t, int32_t);
struct Ctx289 { FnLog* L; uint32_t dval; };
static uint32_t cb289(void* c, uint32_t rva, const uint32_t* r, const uint32_t* s) {
    Ctx289* x = (Ctx289*)c;
    if (rva == 0x46230) *(uint32_t*)(uintptr_t)s[1] = x->dval;   // hookw 0x46230 1: the callee fills the caller's out buffer
    return fnlog_call(x->L, rva, r, s);
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_289a0(uint32_t* r) {
    const uint32_t* st = (const uint32_t*)(uintptr_t)r[0];
    FnHk t[] = { { 0xc202, 1, "-", -1, 0, 0, 0, st[6] }, { 0xc1ba, 1, "-", -1, 0, 0, 0, st[7] }, { 0x914f0, 3, "-", -1, 0, 0, 2, 0 },
                 { 0xc19c, 1, "-", -1, 0, 0, 0, st[8] }, { 0x560b0, 3, "-", -1, 0, 0, 0, 0 }, { 0xbf5f0, 1, "-", -1, 0, 0, 0, 0 },
                 { 0x8b760, 0, "-", -1, 0, 0, 0, st[9] }, { 0x46230, 3, "ac", -1, 0, 0, 2, st[10], 1 }, { 0xd1ec, 0, "c", -1, 0, 0, 0, 0 },
                 { 0xd384, 3, "cd", -1, 0, 0, 0, 0, 2 } };
    t[5].seq[0] = st[12]; t[5].seq[1] = st[13]; t[5].nseq = 2;
    FnLog L = { (uint32_t*)(uintptr_t)r[1], t, 10 }; L.lg[0] = 0;
    Ctx289 x = { &L, st[11] };
    d2c_289a0({ &x, cb289 }, r[2], r[3], st[0], st[1], st[2], (int32_t)st[3], (int32_t)st[4], (int32_t)st[5]);
    r[4] = 0;
}
