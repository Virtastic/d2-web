// src/core/d2client/d2client_batch_b0e70.cpp -- twelve UNMAPPED 1.13c D2Client.dll rows (alpha lane)
// from build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000. Unit/record memory is read through
// the Env ld/st seams at RETAIL offsets (the packed wasm64 layout widens pointers; the host maps them).
// Unrecovered callees are Env function pointers; register args are explicit params. Missile state
// machines (B1D10, B1FC0) have garbled register args in the decompile: marked GUESS.
#include <cstdint>
#include <emscripten.h>

typedef uintptr_t U;
typedef uint32_t (*D2cFn)(U a, U b, U c);   // Ordinal_* / FUN_* seam; unused args are 0

struct D2cEnvB0e70 {
    U dt;                                    // *sgptDataTables
    uint32_t (*ld32)(U base, uint32_t off);
    int32_t  (*ld16s)(U base, uint32_t off);
    uint32_t (*ld8)(U base, uint32_t off);
    U        (*ldp)(U base, uint32_t off);
    void (*st32)(U base, uint32_t off, uint32_t v);
    void (*stp)(U base, uint32_t off, U v);
    void (*tailNext)(U unit);                // FUN_6fb617c0 (0xB17C0)
    void (*tailDone)(U unit);                // FUN_6fb61720 (0xB1720)
    D2cFn o10998, o10178, o10043, f69700, o10505, o11016, o10692;           // B0E70
    D2cFn f1380, o11029, o10240, f60450;                                    // B1080
    D2cFn o11018, f560d0, f1660, f1630, f51860, f602e0;                     // B1270
    D2cFn f25d0, f38c20, o10398, f585f0;                                    // B1720
    void (*f56ef0)(U tab, U lo7, U id, U kind);                              // FUN_6fb56ef0 (0x56EF0), 4 args
    D2cFn f60df0, o10331, o10750, o10867, f1400, o10057;                    // B1AF0
    D2cFn o10818, f617c0;                                                   // B1CC0
    D2cFn o10985, o10441;                                                   // B24A0
    D2cFn o10737, f38a70, tick;                                             // B2500
    D2cFn o10614, o10286, o10334, o10513, o10637, o10019;                   // B25D0
    D2cFn f1330, f77bf0, o10488, o10265, o10024, o10522, exit_;             // B1D10/B1FC0
    U bitMask8;                                                              // gdwBitMasks[8]
    U tabB9A08, tabBA608, hashBA008;                                         // unit tables
    U jmpTab8D8A0;                                                           // handler table
    int32_t jmpCount;                                                        // DAT_6fb8da0c
    uint32_t* gTick; uint32_t* gB9d0; uint32_t* gB9d4; uint32_t* gB9d8; uint32_t* gBefc; uint32_t* gC3b8;
};

static U rec(const D2cEnvB0e70& E, U unit) {   // monster-record lookup, 0x1a4 stride; 0 on any bound miss
    if (!unit) return 0;
    int32_t i = (int32_t)E.ld32(unit, 4);
    if (i < 0 || i >= (int32_t)E.ld32(E.dt, 0xb6c)) return 0;
    return E.ldp(E.dt, 0xb64) + (U)i * 0x1a4;
}
static U sub30(const D2cEnvB0e70& E, U unit) {  // unit+0x54 -> +0x30, or 0
    if (!unit) return 0;
    U p = E.ldp(unit, 0x54);
    return p ? E.ldp(p, 0x30) : 0;
}

// 1.13c D2Client+0xB0E70: unit teardown; esi = unit.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b0e70(D2cEnvB0e70* e, U esi) {
    D2cEnvB0e70& E = *e;
    E.o10998(0,0,0); E.o10178(0,0,0);
    if (esi && E.ldp(esi, 0x54) && E.ldp(E.ldp(esi, 0x54), 0x30)) {
        E.o10043(100, 0, 0);
        if (E.ldp(esi, 0x54)) E.stp(E.ldp(esi, 0x54), 0x30, 0);
    }
    E.f69700(0,0,0);
    if (*E.gC3b8) { E.o10505(0,0,0); E.o11016(0,0,0); }
    E.o10692(0, E.ld32(esi, 0x2c), 0);
    E.st32(esi, 0x2c, 0);
}

// 1.13c D2Client+0xB1080 (fastcall ecx = unit): always 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_b1080(D2cEnvB0e70* e, U unit) {
    D2cEnvB0e70& E = *e;
    U r = rec(E, unit);
    if (r && E.ld16s(r, 0x2c) >= 0) {
        if (E.ld16s(r, 0x2c) < E.ld16s(r, 0x2e)) E.f1380(unit + 0x20, 0, 0);
        uint32_t u = E.o11029(unit, 0, 0);
        E.o10240(unit, u, 0);
        E.f60450(0,0,0);
    }
    return 1;
}

// 1.13c D2Client+0xB1270 (eax = unit): returns 0 when the packet was sent, else 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_b1270(D2cEnvB0e70* e, U unit) {
    D2cEnvB0e70& E = *e;
    U r = rec(E, unit);
    if (!r) return 1;
    uint32_t v = E.ld32(r, 0x3c);
    if (E.o11018(unit,0,0) & 4) return 1;
    U p = E.f560d0(0,0,0);
    if (!p) return 1;
    uint32_t y = E.f1660(unit,0,0);
    uint32_t x = E.f1630(unit,0,0);
    E.f51860(p, x, y);  // GUESS: (p, x, y, v, 0xA283, cb 0x6FB509F0) -- 6 args collapse into the seam
    (void)v;
    E.f602e0(0,0,0);
    return 0;
}

// 1.13c D2Client+0xB1720 (esi = unit): per-unit post-tick; ends the walk.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b1720(D2cEnvB0e70* e, U esi) {
    D2cEnvB0e70& E = *e;
    if (!esi) return;
    U r = rec(E, esi);
    if (r && E.ld16s(r, 0x12) >= 0) {
        if (E.f25d0(esi,0,0)) E.f38c20(esi, 0, 0);
    }
    if (E.o10398(0,0,0)) E.f585f0(0,0,0);
    uint32_t type = E.ld32(esi, 0), id = E.ld32(esi, 0xc);
    if ((E.ld32(esi, 0xc4) >> 0x15) & 1) E.f56ef0(E.tabB9A08 + (U)type * 0x200, id & 0x7f, id, type);
    else                                  E.f56ef0(E.tabBA608 + (U)type * 0x80,  id & 0x7f, id, type);
}

// 1.13c D2Client+0xB1AF0 (eax = unit): mode dispatch through the 0x6FB8D8A0 handler table.
// handlerCall(idx) runs table[idx]; the host owns the indirect jump.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b1af0(D2cEnvB0e70* e, U unit, void (*handlerCall)(int32_t idx)) {
    D2cEnvB0e70& E = *e;
    U r = rec(E, unit);
    if (r) {
        if (E.ld8(r, 0x131)) E.f60df0(unit,0,0);
        uint32_t k = 0;
        if (E.o10331(0,0,0) == 0) {
            int32_t t = (int32_t)E.ld32(unit, 0);
            if (t != 2 && (t < 4 || t > 5) && E.ld32(unit, 0x2c)) E.o10750(E.ld32(unit, 0x2c),0,0);
            t = (int32_t)E.ld32(unit, 0);
            if (t != 2 && (t < 4 || t > 5) && E.ld32(unit, 0x2c)) E.o10867(E.ld32(unit, 0x2c),0,0);
            k = E.f1400(0,0,0);
        }
        if ((E.ld8(E.bitMask8, 0) & E.ld8(r, 5)) || (k = E.o10057(k,0,0)) == 0) {
            int32_t s = E.ld16s(r, 8);
            if (s < 1 || E.jmpCount <= s) return;
            handlerCall(s);
            return;
        }
    }
    E.tailDone(0);
}

// 1.13c D2Client+0xB1CC0 (fastcall ecx = unit): range test on the sub-struct's +0xC >> 11.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b1cc0(D2cEnvB0e70* e, U unit) {
    D2cEnvB0e70& E = *e;
    int32_t lo = (int32_t)E.o11018(unit,0,0), hi = (int32_t)E.o10818(unit,0,0);
    U s = sub30(E, unit);
    int32_t v = s ? (int32_t)E.ld32(s, 0xc) >> 11 : 0;
    if (-lo <= v && v <= hi) E.tailNext(unit); else E.tailDone(0);
}

// 1.13c D2Client+0xB2480 (fastcall ecx = unit).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b2480(D2cEnvB0e70* e, U unit) {
    D2cEnvB0e70& E = *e;
    if (E.f560d0(unit,0,0)) E.st32(unit, 0x44, 0);
    E.tailNext(unit);
}

// 1.13c D2Client+0xB24A0 (fastcall ecx = unit): mode 8 halves, mode 0x23 doubles the +0x4C short.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b24a0(D2cEnvB0e70* e, U unit) {
    D2cEnvB0e70& E = *e;
    uint32_t m = E.o10985(unit,0,0);
    int32_t w = E.ld16s(unit, 0x4c);
    if (m == 8) {
        U s = sub30(E, unit);
        if (s) { E.st32(s, 0x1c, 0); E.st32(s, 0x20, 0); E.st32(s, 0x24, 0x100); }
        E.o10441(unit, (uint32_t)(w >> 1), 0);
    } else if (m == 0x23) {
        E.o10441(unit, (uint32_t)(w << 1), 0);
    }
    E.tailNext(unit);
}

// 1.13c D2Client+0xB2500 (fastcall ecx = unit): id 0x96 arms the timer globals, 0x32 calls FUN_6fb38a70.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b2500(D2cEnvB0e70* e, U unit) {
    D2cEnvB0e70& E = *e;
    uint32_t id = E.o10737(unit,0,0);
    if (id == 0x96) {
        *E.gTick = E.tick(0,0,0); *E.gB9d0 = 0; *E.gB9d4 = 4000; *E.gB9d8 = 0; *E.gBefc = 0x19;
    } else if (id == 0x32) {
        E.f38a70(0,0,0);
    }
    E.tailNext(unit);
}

// 1.13c D2Client+0xB25D0 (fastcall ecx = unit): random-walk step; seed is the u64 at unit+0x20
// (lo*0x6AC690C5 + hi, the engine's LCG).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b25d0(D2cEnvB0e70* e, U unit) {
    D2cEnvB0e70& E = *e;
    U r = rec(E, unit);
    if (!r) { E.tailDone(0); return; }
    int32_t every = (int32_t)E.ld32(r, 0x38);
    if (every < 1) every = 1;
    if ((int32_t)E.o10985(unit,0,0) % every != 0) { E.tailNext(unit); return; }
    E.o11018(unit,0,0); E.o10614(0,0,0);
    uint32_t vv = E.o10818(unit,0,0);
    int32_t a = (int16_t)vv, b = (int16_t)(vv >> 16);
    uint64_t s = (uint64_t)E.ld32(unit, 0x20) * 0x6AC690C5ull + E.ld32(unit, 0x24);
    E.st32(unit, 0x20, (uint32_t)s); E.st32(unit, 0x24, (uint32_t)(s >> 32));
    int32_t p, q;
    if ((s & 1) == 0) { q = -a; p = b; } else { p = -b; q = a; }
    p += a * 4; q += b * 4;
    int32_t dx = (p + ((p >> 31) & 3)) >> 2, dy = (q + ((q >> 31) & 3)) >> 2;
    if (dx == 0) dx = 1;
    if (dy == 0) dy = 1;
    uint32_t ny = E.f1660(unit,0,0) + dy, nx = E.f1630(unit,0,0) + dx;
    uint32_t path = E.ld32(unit, 0x2c);
    E.o10286(path, nx, ny);
    E.o10334(path, unit, 0);
    E.o10637(unit, E.o10513(0,0,0), 0);
    E.o10019(unit, (uint32_t)(dy * 0x10000) + ((uint32_t)dx & 0xffff), 0);
    E.tailNext(unit);
}

// Shared by B1D10/B1FC0: find the owner unit (type 3) in the hash at 0x6FBBA008 by id Ordinal_11018.
static U owner(const D2cEnvB0e70& E, U unit) {
    uint32_t id = E.o11018(unit,0,0);
    for (U o = E.ldp(E.hashBA008, (id & 0x7f) * 8); o; o = E.ldp(o, 0xE4))   // GUESS: next at retail +0xE4
        if (E.ld32(o, 0xc) == id) return o;
    return 0;
}
static void die(const D2cEnvB0e70& E) { E.o10024(0x6FB8359Full, E.o10265(0x5a,0,0), 0); E.exit_(-1,0,0); }

// 1.13c D2Client+0xB1D10 (fastcall ecx = missile): esi = mode (0/1), GUESS (unaff_ESI in the decompile).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b1d10(D2cEnvB0e70* e, U m, uint32_t esi) {
    D2cEnvB0e70& E = *e;
    E.o10818(m,0,0);
    if (!E.ld32(m, 0x2c)) { E.tailNext(m); return; }
    U o = owner(E, m);
    if (!o) { E.tailNext(m); return; }
    if (E.ld32(o, 0) != 3) { die(E); return; }
    uint32_t st = E.o11018(o,0,0);
    U seedp = m + 0x20;
    if (esi == st || (int32_t)st < 2 || E.f1330(seedp,0,0) != 0) {
        if (esi == 0) {
            uint64_t s = (uint64_t)E.ld32(m, 0x20) * 0x6AC690C5ull + E.ld32(m, 0x24);
            E.st32(m, 0x20, (uint32_t)s); E.st32(m, 0x24, (uint32_t)(s >> 32));
            if ((uint32_t)s % 10 == 0) {
                E.o10019(m, 1, 0); E.f77bf0(0,0,0);
                E.o10488(m, 0xf00, 0x19e6);
                U sp = E.ldp(m, 0x54);
                U x = sp ? E.ldp(sp, 0x30) : 0;
                if (E.f1330(seedp,0,0) != 0 && x) {
                    E.st32(x, 0x10, 0); E.st32(x, 0x14, 0); E.st32(x, 0x18, 0x800);
                }
            }
            E.tailNext(m); return;
        }
        if (esi != 1) { E.tailNext(m); return; }
        int32_t dx = (int32_t)(E.f1630(m,0,0) - E.f1630(o,0,0));
        int32_t dy = (int32_t)(E.f1660(m,0,0) - E.f1660(o,0,0));
        if (E.ld8(m, 0xc) & 2) { dx = -dx; dy = -dy; }
        if (!dx && !dy) { E.tailNext(m); return; }
        E.o10286(m, E.f1630(m,0,0) - dy, E.f1660(m,0,0) + dx);
    } else {
        E.o10019(m, st, 0);
        if (st != 2) { E.tailNext(m); return; }
        E.o10488(m, 0xf00, 0x19db);
        E.o10286(m, 0x3b1f, 0x170c);
    }
    E.o10334(m, o, 0);
    E.tailNext(m);
}

// 1.13c D2Client+0xB1FC0 (fastcall ecx = missile): edi = mode, GUESS. The state switch (2/3/4) is
// garbled in the decompile; reconstructed as st = owner state, sub30(m) the missile's control block.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b1fc0(D2cEnvB0e70* e, U m, uint32_t edi) {
    D2cEnvB0e70& E = *e;
    E.o10818(m,0,0);
    if (!E.ld32(m, 0x2c)) { E.tailNext(m); return; }
    U o = owner(E, m);
    if (!o) { E.tailNext(m); return; }
    if (E.ld32(o, 0) != 3) { die(E); return; }
    uint32_t st = E.o11018(o,0,0);
    uint32_t dist = E.o10522(m, o, 0);
    uint64_t s = (uint64_t)E.ld32(m, 0x20) * 0x6AC690C5ull + E.ld32(m, 0x24);
    E.st32(m, 0x20, (uint32_t)s); E.st32(m, 0x24, (uint32_t)(s >> 32));
    uint32_t lim = (uint32_t)s % 5; if (lim < 2) lim = 1;
    U x = sub30(E, m);
    if (edi == st || (int32_t)st < 2) {
        if ((int32_t)dist < (int32_t)lim && edi == 0) {
            E.o10019(m, 1, 0); E.f77bf0(0,0,0);
            if (x) { E.st32(x, 0x10, 0); E.st32(x, 0x14, 0); E.st32(x, 0x18, 0x800); }
        } else {
            uint32_t line;
            if (edi == 1) {
                int32_t dx = (int32_t)(E.f1630(m,0,0) - E.f1630(o,0,0));
                int32_t dy = (int32_t)(E.f1660(m,0,0) - E.f1660(o,0,0));
                if (!dx && !dy) { E.tailNext(m); return; }
                E.o10286(m, E.f1630(m,0,0) - dy, E.f1660(m,0,0) + dx);
                E.o10488(m, 0xa00, 0x179e); line = 0;
            } else if (edi == 3) {
                E.o10286(m, E.f1630(o,0,0), E.f1660(o,0,0));
                E.o10488(m, 0x200, 0x17a7); line = 0;
            } else { E.tailNext(m); return; }
            (void)line;
            E.o10334(m, o, 0);
        }
    } else {
        E.o10019(m, st, 0);
        if (st == 2) {
            E.o10488(m, 0, 0x1774); E.f77bf0(0,0,0);
            if (x) { E.st32(x, 0x10, 0); E.st32(x, 0x14, 0); E.st32(x, 0x18, 0x3000); }
            uint32_t r = E.f1330(m + 0x20,0,0);
            if (x) { E.st32(x, 0x28, 0); E.st32(x, 0x2c, 0); E.st32(x, 0x30, r + 0xaa); E.st32(x, 0, E.ld32(x, 0) | 0x20); }
            E.o10286(m, E.f1630(o,0,0), E.f1660(o,0,0));
            E.o10488(m, 0x300, 0x177c);
            E.o10334(m, o, 0);
        } else if (st == 3) {
            E.f77bf0(0,0,0);
            uint32_t r = E.f1330(m + 0x23,0,0);
            if (x) { E.st32(x, 0x10, 0); E.st32(x, 0x14, 0); E.st32(x, 0x18, 0xffffd000u);
                     E.st32(x, 0x28, 0); E.st32(x, 0x2c, 0); E.st32(x, 0x30, r);
                     E.st32(x, 0, E.ld32(x, 0) & 0xffffffdfu); }
            E.o10334(m, o, 0);
        } else if (st == 4) {
            E.f77bf0(0,0,0);
            if (x) { E.st32(x, 0x10, 0); E.st32(x, 0x14, 0); E.st32(x, 0x18, 0xa000);
                     E.st32(x, 0x28, 0); E.st32(x, 0x2c, 0); E.st32(x, 0x30, 400);
                     E.st32(x, 0, E.ld32(x, 0) | 0x20); }
            E.o10334(m, o, 0);
        }
    }
    E.tailNext(m);
}
