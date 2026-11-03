// d2client+0x28dc0 probe: r[0] = st (see D2c28dc0G in d2client_28dc0.cpp, then 16 cf22 count, 17 c262, 18 c220, 19 cf1c, 20 strlen, 21..24 cf46 out),
// r[1] = hooklog, r[2] = the player unit blob, r[3] = key table, r[5] = sq (hook value streams), r[6..8] = items.
#include <emscripten.h>
#include "d2client_fnlog.h"
extern "C" void d2c_28dc0(const D2cSvc&, uint32_t*, uint32_t, uint32_t, const uint8_t*);
struct Ctx28 { FnLog* L; uint32_t* st; uint32_t base[8]; uint32_t size[8]; const uint32_t* sq; const uint32_t* items; uint32_t used; };
static uint32_t tag28(const Ctx28* x, uint32_t v) {   // a pointer into a tuple blob is logged 0xB1<blob><offset>
    for (int b = 0; b < 8; ++b) if (v >= x->base[b] && v < x->base[b] + x->size[b]) return 0xB1000000u | (uint32_t)b << 16 | (v - x->base[b]);
    return v;
}
static uint32_t cb28(void* c, uint32_t rva, const uint32_t* r, const uint32_t* s) {
    Ctx28* x = (Ctx28*)c;
    if (rva == 0xceab0) return x->st[20];
    if (rva == 0xcf22) { uint32_t* o = (uint32_t*)(uintptr_t)s[2]; o[0] = 0; o[1] = x->st[16]; }
    if (rva == 0xcf46) { uint32_t* o = (uint32_t*)(uintptr_t)s[2]; o[0] = x->st[21]; o[1] = 0; o[2] = x->st[23]; o[3] = x->st[24]; }
    uint32_t rr[6], ss[8];
    for (int i = 0; i < 6; ++i) rr[i] = tag28(x, r[i]);
    for (int i = 0; i < 8; ++i) ss[i] = tag28(x, s[i]);
    const uint32_t v = fnlog_call(x->L, rva, rr, ss);
    if (rva == 0xcf28) {   // 0 = null, k+1 = item k, in the order the sq stream lists
        const uint32_t k = x->sq[x->used < 8 ? x->used++ : 7];
        return k ? x->items[k - 1] : 0;
    }
    return v;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_28dc0(uint32_t* r) {
    uint32_t* st = (uint32_t*)(uintptr_t)r[0];
    const uint32_t* sq = (const uint32_t*)(uintptr_t)r[5];
    Ctx28 x = { nullptr, st, { r[0], r[1], r[2], r[3], r[5], r[6], r[7], r[8] }, { 104, 16384, 128, 1140, 384, 64, 64, 64 }, sq, &r[6], 0 };
    FnHk t[] = { { 0xd3a8, 0, "c", -1, 0, 0, 0, 0 }, { 0xc262, 2, "-", -1, 0, 0, 0, st[17] }, { 0xc220, 1, "-", -1, 0, 0, 0, st[18] }, { 0xcf1c, 1, "-", -1, 0, 0, 0, st[19] },
                 { 0x265e0, 0, "-", -1, 0, 0, 0, 0 }, { 0xd096, 6, "-", 0, 18, 0xFFFFFFFFu, 1, 0 }, { 0xcf22, 3, "-", -1, 0, 0, 4, 0 }, { 0xcf28, 2, "-", -1, 0, 0, 0, 0 },
                 { 0xc3be, 1, "-", -1, 0, 0, 0, 0 }, { 0xc1a8, 4, "-", -1, 0, 0, 0, 0 }, { 0xc814, 1, "-", -1, 0, 0, 0, 0 }, { 0xca12, 2, "-", -1, 0, 0, 0, 0 },
                 { 0xc1fc, 2, "-", -1, 0, 0, 0, 0 }, { 0xd390, 3, "-", -1, 0, 0, 0, 0 }, { 0xcf46, 4, "-", -1, 0, 0, 4, 0 }, { 0xd0ba, 6, "h", -1, 0, 0, 0, 0 },
                 { 0x6b5a0, 2, "b", -1, 0, 0, 0, 0 }, { 0xd35a, 0, "c", -1, 0, 0, 0, 0, 2 }, { 0x39f30, 0, "d", -1, 0, 0, 0, 0 }, { 0xd37e, 3, "cd", -1, 0, 0, 0, 0, 2 },
                 { 0x277e0, 0, "-", -1, 0, 0, 0, 0 } };
    auto seq = [&](FnHk& k, int off, int n) { for (int i = 0; i < n; ++i) k.seq[i] = sq[off + i]; k.nseq = (uint8_t)n; };
    seq(t[8], 48, 8); seq(t[9], 8, 8); seq(t[10], 16, 16); seq(t[11], 32, 8); seq(t[12], 40, 8); seq(t[13], 80, 16); seq(t[17], 56, 16); seq(t[18], 72, 8);
    FnLog L = { (uint32_t*)(uintptr_t)r[1], t, sizeof t / sizeof t[0] }; L.lg[0] = 0;
    x.L = &L;
    d2c_28dc0({ &x, cb28 }, st, r[2], r[7], (const uint8_t*)(uintptr_t)r[3]);
    r[4] = 0;
}
