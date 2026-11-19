// src/core/d2client/d2client_batch_b2720.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (missile UNIT\Missile.cpp
// per-frame/event callbacks, RVA 0xB2720-0xB30EE) from the cached decompiles. Alpha batch lane: marker
// only, no spec, no vector. Base 0x6FAB0000.
//
// Every function opens with the same missiles.txt lookup: idx = unit+4 (dwClassId), bounds-checked against
// sgptDataTables+0xB6C (count), record = *(sgptDataTables+0xB64) + idx*0x1A4; the seam miss(idx) returns
// that record address or 0. Unrecovered callees are Env function pointers named by retail RVA/ordinal;
// retail's 32-bit field reads/writes go through rd32/rd16/wr32 so wasm64 pointers stay uintptr_t.
// Guesses are marked GUESS.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct MissB2720Env {
    uintptr_t (*miss)(int32_t idx);                    // missiles.txt record or 0 (sgptDataTables+0xB64/0xB6C)
    int32_t   (*rd32)(uintptr_t a);
    int32_t   (*rd16s)(uintptr_t a);                   // signed short
    uint32_t  (*rd8)(uintptr_t a);
    void      (*wr32)(uintptr_t a, int32_t v);
    int32_t   (*o10985)(uintptr_t u);                  // Ordinal_10985 (frame counter)
    int32_t   (*o10737)(uintptr_t u);                  // Ordinal_10737
    void      (*o10970)(uintptr_t u, int32_t v);       // Ordinal_10970 (set flags/value)
    void      (*o10558)(void);                         // Ordinal_10558
    int32_t   (*o11018)(uintptr_t u);                  // Ordinal_11018
    int32_t   (*o10818)(uintptr_t u);                  // Ordinal_10818
    void      (*o10286)(int32_t room, int32_t x, int32_t y);   // Ordinal_10286 (set pos)
    void      (*o10334)(int32_t room, uintptr_t u, int32_t f); // Ordinal_10334
    void      (*o10637)(uintptr_t u, int32_t v);
    void      (*o10019)(uintptr_t u, int32_t v);
    int32_t   (*o10392)(int32_t room);                 // Ordinal_10392 (returns unit ptr, 32-bit slot)
    int32_t   (*o10522)(uintptr_t a, uintptr_t b);     // Ordinal_10522 (distance)
    uint32_t  (*o10265)(uint32_t id);
    void      (*o10024)(const char* s, uint32_t v);
    int32_t   (*o10511)(uintptr_t p);
    int32_t   (*o10601)(int32_t v, const char* file, int32_t line);
    int32_t   (*o10240get)(uintptr_t u);               // Ordinal_10240(u)
    void      (*o10240set)(uintptr_t u, int32_t v);    // Ordinal_10240(u,v)
    int32_t   (*o10203)(int32_t v);
    int32_t   (*o11029)(uintptr_t u);
    void      (*o11121)(uintptr_t u, uintptr_t p);
    void      (*o10759)(uintptr_t u);
    int32_t   (*o10426)(uintptr_t u);
    void      (*o10488)(int32_t v, int32_t z, const char* file, int32_t line);
    int32_t   (*o10651)(uintptr_t u);
    int32_t   (*o11142)(uintptr_t u);
    int32_t   (*o10331)(uintptr_t u);
    int32_t   (*o10057)(int32_t v);
    int32_t   (*x1630)(uintptr_t u);                   // 0x1630 (x)
    int32_t   (*x1660)(uintptr_t u);                   // 0x1660 (y)
    uint32_t  (*x1380)(uintptr_t p);                   // 0x1380 (on unit+0x20)
    void      (*bUpdate)(uintptr_t u);                 // 0xB17C0 (normal return path)
    void      (*bBad)(void);                           // 0xB1720 (bad-record path)
    void      (*bFree)(uintptr_t u, int32_t f);        // 0xB1420
    void      (*b0a20)(void);                          // 0xB0A20
    void      (*b0450)(void);                          // 0xB0450 (ECX GUESS: unused)
    uintptr_t (*a60d0)(uintptr_t u);                   // 0xA60D0 (FUN_6fb560d0, returns int* or 0)
    int32_t   (*a5440)(void);                          // 0xA5440
    void      (*a6ef0)(int32_t tblOff, int32_t a, int32_t b, int32_t c); // 0xA6EF0 (tblOff = idx*0x200 into 0x6FBB9A08)
    uintptr_t (*a5f70)(void);                          // 0xA5F70
    uint32_t  (*a62a0)(uintptr_t p);                   // 0xA62A0
    void      (*afF10)(uintptr_t player, int32_t sk, int32_t x, int32_t y, int32_t z, uintptr_t u); // 0xAFF10
    void      (*c190)(void);                           // 0x2A0B0 (FUN_6fada0b0)
    void      (*exit_)(int32_t);                       // _exit
    int32_t   (*gDat9960)(void);                       // DAT_6fbc9960
    int32_t   (*gDatC418)(void);                       // DAT_6fbcc418
    int32_t   (*gDat9e14)(void);                       // DAT_6fba9e14
    int32_t   (*gDat995c)(void);                       // DAT_6fbc995c
    int32_t   (*gDat9e18)(void);                       // DAT_6fba9e18
};

// 1.13c D2Client+0xB2720
EMSCRIPTEN_KEEPALIVE void d2_client_b2720(uintptr_t u, const MissB2720Env* e) {
    uintptr_t r;
    if (!u || !(r = e->miss(e->rd32(u + 4))) || e->rd16s(r + 0x1e) < 0) { e->bBad(); return; }
    int32_t n = e->o10985(u);
    int32_t m = e->rd32(r + 0x58);
    if (m < 1) m = 1;
    if (n % m == 0) e->b0a20();
    e->bUpdate(u);
}

// 1.13c D2Client+0xB2790
EMSCRIPTEN_KEEPALIVE void d2_client_b2790(uintptr_t u, const MissB2720Env* e) {
    if (!u) { e->a6ef0(6 * 0x200, -1 & 0x7f, -1, 6); return; }
    int32_t cls = e->rd32(u + 4), id = e->rd32(u), f3 = e->rd32(u + 0xc);
    uintptr_t r = e->miss(cls);
    if (r && e->rd16s(r + 0x1e) >= 0) {
        uintptr_t p = e->a60d0(u);
        if (p) {
            int32_t q = e->o10511(p);
            if (q) {
                int32_t a = e->o10601(q, "..\\Source\\D2Client\\UNIT\\Missile.cpp", 0x8ed);
                if (a == e->o10240get(u)) {
                    if (e->o10203(e->rd32(u + 0x2c))) {
                        e->o10240set(u, e->o11029(u));
                        e->b0450();
                    }
                    e->o11121(u, p);
                    e->bUpdate(u);
                    return;
                }
            }
        }
        e->a6ef0(id * 0x200, f3 & 0x7f, f3, id);
        return;
    }
    e->a6ef0(id * 0x200, f3 & 0x7f, f3, id);
}

// 1.13c D2Client+0xB2890
EMSCRIPTEN_KEEPALIVE void d2_client_b2890(uintptr_t u, const MissB2720Env* e) {
    uintptr_t p = e->a60d0(u);
    if (!p) { e->bBad(); return; }
    int32_t k = e->rd32(p);
    if (k >= 0 && k < 2 && e->a5440()) { e->bFree(u, 0); return; }
    e->o11121(u, p); // GUESS: retail passes the register left by 0xA5440 (EDX); p assumed
    e->o10759(u);
}

static void b28e0_body(uintptr_t u, const MissB2720Env* e, uintptr_t r, int32_t a, int32_t b) {
    uintptr_t s = e->rd32(u + 0x54) ? (uintptr_t)(uint32_t)e->rd32(u + 0x54) : 0; // 32-bit ptr slot
    uintptr_t v = s ? (uintptr_t)(uint32_t)e->rd32(s + 0x30) : 0;
    if (v) { e->wr32(v + 4, 0); e->wr32(v + 8, 0); e->wr32(v + 0xc, a << 11); }
    if (s && v) { e->wr32(v + 0x10, 0); e->wr32(v + 0x14, 0); e->wr32(v + 0x18, b * -0x800); }
    (void)r;
    e->o10558();
    e->o10970(u, 0); // GUESS: arg(s) of Ordinal_10970 come from registers; 0 assumed
}

// 1.13c D2Client+0xB28E0   (unit in EAX)
EMSCRIPTEN_KEEPALIVE void d2_client_b28e0(uintptr_t u, const MissB2720Env* e) {
    uintptr_t r;
    if (!u || !(r = e->miss(e->rd32(u + 4)))) { e->bBad(); return; }
    int32_t b = e->rd32(r + 0x5c);
    if (b > 0) b28e0_body(u, e, r, e->rd32(r + 0x58), b);
}

// 1.13c D2Client+0xB2980
EMSCRIPTEN_KEEPALIVE void d2_client_b2980(uintptr_t u, const MissB2720Env* e) {
    if (e->o10737(u) < 100) e->o10970(u, 500);
    e->bUpdate(u);
}

// 1.13c D2Client+0xB29B0
EMSCRIPTEN_KEEPALIVE void d2_client_b29b0(uintptr_t u, const MissB2720Env* e) {
    uintptr_t r;
    if (!u || !(r = e->miss(e->rd32(u + 4)))) { e->bBad(); return; }
    int32_t step = e->rd32(r + 0x5c);
    if (step < 1) step = 1;
    int32_t n = e->o10985(u);
    if (n < e->rd32(r + 0x58) && n % step == 0) {
        int32_t a = e->o11018(u), b = e->o10818(u);
        int32_t lo = (a - b) / 2, hi = (a + b) / 2;
        int32_t y = e->x1660(u) + hi;
        int32_t x = e->x1630(u) + lo;
        int32_t room = e->rd32(u + 0x2c);
        e->o10286(room, x, y);
        e->o10334(room, u, 0);
        e->o10637(u, lo);
        e->o10019(u, hi);
    }
    e->bUpdate(u);
}

// 1.13c D2Client+0xB2A80
EMSCRIPTEN_KEEPALIVE void d2_client_b2a80(uintptr_t u, const MissB2720Env* e) {
    uintptr_t r;
    if (!u || !(r = e->miss(e->rd32(u + 4))) || !e->a60d0(u)) { e->bBad(); return; }
    e->c190();
    int32_t room = e->rd32(u + 0x2c);
    if (!room) {
        e->o10024("", e->o10265(0x946)); // text at DAT_6fb8359f (GUESS: file/line string)
        e->exit_(-1);
        return;
    }
    int32_t t = e->o10392(room);
    uintptr_t tgt = (uintptr_t)(uint32_t)t;
    if (!tgt && !(tgt = e->a5f70())) { e->bUpdate(u); return; }
    int32_t per = e->rd32(r + 0x60);
    if (per < 1) per = 5;
    if (e->o10985(u) % per == 0) {
        if (e->o10522(u, tgt) <= e->rd32(r + 100)) {
            int32_t ux = e->x1630(u), uy = e->x1660(u), tx = e->x1630(tgt), ty = e->x1660(tgt);
            int32_t sp = e->rd32(r + 0x68);
            if (sp < 1) sp = 1;
            int32_t dx = sp; if (tx <= ux) { dx = 0; if (tx < ux) dx = -sp; }
            int32_t dy = sp; if (ty <= uy) { dy = 0; if (ty < uy) dy = -sp; }
            int32_t ny = e->x1660(u) + dy;
            int32_t nx = e->x1630(u) + dx;
            e->o10286(room, nx, ny);
            e->o10334(room, u, 0);
        }
    }
    e->bUpdate(u);
}

// 1.13c D2Client+0xB2BF0
EMSCRIPTEN_KEEPALIVE void d2_client_b2bf0(uintptr_t u, const MissB2720Env* e) {
    uintptr_t r;
    if (!u || !(r = e->miss(e->rd32(u + 4)))) { e->bBad(); return; }
    static const struct { int32_t sk, fr; } T[3] = { {0x1e, 0x58}, {0x20, 0x5c}, {0x22, 0x60} };
    for (int i = 0; i < 3; i++) {
        int32_t sk = e->rd16s(r + T[i].sk);
        if (sk > 0 && e->o10985(u) == e->rd32(r + T[i].fr)) {
            int32_t a = e->o11029(u);
            e->o10240set(u, a);
            int32_t b = e->o10240get(u); // GUESS: retail's uVar3 is Ordinal_10240's return
            int32_t y = e->x1660(u), x = e->x1630(u);
            e->afF10(e->a60d0(u), sk, x, y, b, u);
        }
    }
    e->bUpdate(u);
}

// 1.13c D2Client+0xB2E00   (unit in EAX)
EMSCRIPTEN_KEEPALIVE void d2_client_b2e00(uintptr_t u, const MissB2720Env* e) {
    uintptr_t r;
    if (!u || !(r = e->miss(e->rd32(u + 4)))) { e->bBad(); return; }
    int32_t b = e->rd32(r + 0x5c);
    if (b < 1) b = 1;
    int32_t a = e->rd32(r + 0x58);
    if (a < b) a = b;
    b28e0_body(u, e, r, a, b);
}

// 1.13c D2Client+0xB2EB0
EMSCRIPTEN_KEEPALIVE void d2_client_b2eb0(uintptr_t u, const MissB2720Env* e) {
    uintptr_t r;
    if (!u || !(r = e->miss(e->rd32(u + 4)))) { e->bBad(); return; }
    uintptr_t p = e->a60d0(u);
    if (e->o10057(e->o10331(u)) != 0) { e->bBad(); return; }
    if (e->o11018(u) & 1) {
        e->c190();
        int32_t t = e->o10392(e->rd32(u + 0x2c));
        if (t && (e->a5440() || (p && e->a62a0(p) == 0))) t = 0;
        int32_t per = e->rd32(r + 0x58);
        if (per < 1) per = 5;
        if (t && e->o10985(u) % per == 0) {
            int32_t d = e->o10522(u, (uintptr_t)(uint32_t)t);
            if (d > 3 && d < 0x19) e->o10334(e->rd32(u + 0x2c), u, 0);
        }
    }
    e->bUpdate(u);
}

// 1.13c D2Client+0xB2F90
EMSCRIPTEN_KEEPALIVE void d2_client_b2f90(uintptr_t u, const MissB2720Env* e) {
    uintptr_t r;
    if (!u || !(r = e->miss(e->rd32(u + 4)))) { e->bBad(); return; }
    int32_t fr = e->rd32(u + 0x44) >> 8;
    uint32_t last = e->rd8(r + 0x181);
    if (fr == (int32_t)last - 1) {
        e->wr32(u + 0x44, (int32_t)((e->x1380(u + 0x20) + fr) * 0x100));
        e->bUpdate(u);
        return;
    }
    if ((uint32_t)e->o10737(u) == last) {
        int32_t v = (int32_t)last - 3;
        if (v < 0) v = 0;
        e->wr32(u + 0x44, v << 8);
        e->bUpdate(u);
        return;
    }
    if (e->o10737(u) < (int32_t)e->rd8(r + 0x181)) {
        fr -= 2;
        if (fr < 0) fr = 0;
        e->wr32(u + 0x44, fr << 8);
    }
    e->bUpdate(u);
}

// 1.13c D2Client+0xB3060
EMSCRIPTEN_KEEPALIVE void d2_client_b3060(uintptr_t u, const MissB2720Env* e) {
    if (e->o10426(u)) {
        if (u) e->wr32(u + 0xc4, e->rd32(u + 0xc4) | 0x10000);
        e->o10488(e->rd32(u + 0x2c), 0, "..\\Source\\D2Client\\UNIT\\Missile.cpp", 0x7f);
    }
    int32_t d = e->gDat9960() - e->gDatC418();
    int32_t x = e->o10651(u) - d;
    if (x >= 0 && x <= e->gDat9e14()) {
        int32_t y = e->o11142(u) - (e->gDat995c() - 8);
        if (y >= 0 && y <= e->gDat9e18() + 0x40) {
            e->o10970(u, 0x80);
            e->bUpdate(u);
            return;
        }
    }
    e->bFree(u, 0);
}
