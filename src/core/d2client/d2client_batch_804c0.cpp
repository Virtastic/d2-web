// src/core/d2client/d2client_batch_804c0.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (0x804C0..0x82490: rain/snow
// particle update, unit-name cell lookups, item-pickup helpers) reimplemented from the cached decompiles.
// Alpha batch lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector. Base 0x6FAB0000.
// Unrecovered callees (ordinals, other RVAs) are host seams (the d2client_batch_5d110.cpp pattern).
// Register-passed "unaff_*" decompiler params are explicit arguments here.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

template <class T> static T rd(const uint8_t* p, size_t o) { T v; memcpy(&v, p + o, sizeof v); return v; }
template <class T> static void wr(uint8_t* p, size_t o, T v) { memcpy(p + o, &v, sizeof v); }

struct D2cB8G {                 // globals; retail RVA noted
    uint8_t* player;            // +0x10BBFC DAT_6fbcbbfc
    uint8_t* ctx;               // +0x10BF68 DAT_6fbcbf68 (+0x110 count, +0x114 live, +0x11C particle array)
    int32_t  rainMode;          // +0x10BF78
    int32_t  flag7c;            // +0x10BF7C
    int32_t  flag6c;            // +0x10BF6C
    int32_t  cellW, cellH;      // +0x0DBC48 DAT_6fb8bc48, +0x0DBC4C DAT_6fb8bc4c
    int32_t  levelId;           // +0x10C3B8 DAT_6fbcc3b8
    int32_t  tickDX, tickDY;    // +0x10C408 DAT_6fbcc408, +0x10C40C DAT_6fbcc40c
    int32_t  frame;             // +0x1237440 DAT_6fbd3440 (frame counter)
    int32_t  maxLive;           // +0x113934 DAT_6fbc3934
    int32_t  trigHandle;        // +0x113918 DAT_6fbc3918
    const uint8_t* tab84a19;    // DAT_6fb84a19 stride 2
    const uint8_t* tab3940;     // DAT_6fbc3940
    const uint8_t* tab38e0;     // DAT_6fbc38e0
    const uint8_t* tab38d4;     // DAT_6fbc38d4
    const uint8_t* tab38a0;     // DAT_6fbc38a0
    const uint8_t* tab38f8;     // DAT_6fbc38f8
    int32_t  cnt393c, flag38c0, v38bc, v91380, cnt2ac, mode414; // DAT_6fbc393c/38c0/38bc, 6fb91380, 6fbcc2ac, 6fbcc414
    uint32_t s97f8, s985c;      // DAT_6fbc97f8 / DAT_6fbc985c
    uint8_t* dt;                // *sgptDataTables
    uint8_t* hashA;             // &DAT_6fbbae08  128 x ptr chain heads (chain next at +0xE4)
    uint8_t* hashB;             // &DAT_6fbba608  stride 0x80
};

struct D2cB8Env {
    int      (*ord10078)(void);
    uint32_t (*ord10331)(uint32_t);
    uint32_t (*ord10826)(uint32_t);
    uint32_t (*ord11044)(int32_t, int32_t);
    uint32_t (*rnd)(uint64_t* seed);          // FUN_6fab18d0
    int32_t  (*ftol)(void);                   // FUN_6fb7acc4
    void     (*fatal)(uint32_t code);         // Ordinal_10265+10024+_exit(-1)
    uint32_t (*ord10064)(void);
    void     (*ord10065)(void);
    void     (*ord10083)(uint32_t);
    void     (*ord10084)(uint32_t);
    char     (*ord10215)(uint32_t);
    void     (*ord10499)(uint32_t);
    void     (*ord10014)(int,int,int,int,int,int,int,uint32_t);
    void*    (*fn38a70)(int,int,int,int);
    void     (*fn38ba0)(void*,int,int,int);
    void     (*fn2fe80)(void);
    uint8_t* (*ord10695)(int32_t id);         // unit-info lookup
    int      (*ord10106)(void);
    int      (*ord10620)(void);
    int      (*ord10787)(void);
    int      (*ord10744)(void);
    void     (*ord10600)(void);
    uint8_t* (*ord10806)(uint32_t);
    void     (*ord10519)(void);
    void     (*ord10996a)(int);
    void     (*ord10996b)(int,int);
    int      (*ord10202)(void);
    uint32_t (*ord10061)(uint32_t);
    int      (*ord10747)(void);
    void     (*ord10164)(void);
    uint8_t* (*ord10195)(uint8_t* u, int f);
    int      (*ord10680)(uint8_t* it, int a, int b);
    uint8_t* (*ord10900)(uint8_t* it, int f);
    uint8_t* (*ord10460)(uint32_t);
    uint8_t* (*ord10464)(uint8_t*);
    uint8_t* (*ord11147)(uint8_t*);
    void     (*ord11037)(uint8_t*,int,int,int,uint8_t*);
    int      (*ord11145)(uint8_t*,uint8_t*,int,int,int,int,int32_t*);
    uint32_t (*ord10838)(uint8_t*,int);
    int      (*ord10197)(uint32_t);
    void     (*ord10466)(uint32_t,int);
    int      (*ord11017)(uint32_t);
    void     (*fn62d0)(void);
    void     (*fn31aa0)(void);
    void     (*fn56ef0)(uint8_t* tab, uint32_t idx, uint32_t key, int n);
    int      (*ord10708)(uint32_t);
    int      (*ord10828)(void);
    int      (*ord10507)(void);
    void     (*ord10546)(void);
    void     (*ord10978)(void);
    int      (*fn81f60)(uint8_t* unit);
    char     (*ord10417)(void);
    void     (*ord10694)(uint32_t,uint32_t);
    void     (*ord10921)(void);
    void     (*fn19a90)(uint8_t*);
    void     (*ord10605)(void);
    int      (*ord10646)(uint32_t,uint32_t);
    int      (*ord10202b)(int,int,int,const char*);
    void     (*ord11127)(void);
    void     (*ord10608)(uint32_t,char);
};

// 1.13c D2Client+0x804C0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_804c0(D2cB8G* g, const D2cB8Env* e, uint8_t* out1, uint8_t* outB, int32_t idx) {
    int m = e->ord10078();
    bool b = (m < 1) || (m > 3 && m != 6);
    e->ord10826(g->player ? e->ord10331((uint32_t)(uintptr_t)g->player) : 0);
    int32_t lv = (int32_t)e->ord11044(g->levelId, 0);
    if (lv < 0 || lv > 3) e->fatal(0x356);
    if (outB) *outB = g->tab84a19[lv * 2];
    if (!out1) return;
    int32_t a = idx * 12; a += (a >> 31) & 7;
    int32_t i5 = a >> 3;
    if (b) { *out1 = g->tab3940[i5]; return; }
    uint32_t u6 = (uint32_t)(a >> 31);
    if (lv == 2) { *out1 = g->tab38e0[(int32_t)(i5 + (u6 & 3)) >> 2]; return; }
    if (lv == 1 || lv == 3) i5 = (int32_t)(i5 - u6) >> 1;
    else if (lv != 1) { /* lv==0 falls to direct index */ }
    *out1 = g->tab38e0[i5];
}

// 1.13c D2Client+0x805A0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_805a0(D2cB8G* g, const D2cB8Env* e, uint64_t* seed) {
    int32_t i8 = 0;
    if (g->player) {
        uint32_t u = e->ord10826(e->ord10331((uint32_t)(uintptr_t)g->player));
        uint32_t f = e->ord10215(u) != 0;
        e->ord10499(u);
        if (g->flag6c && g->cnt393c) {
            if (--g->cnt393c == 0) {
                if (g->flag38c0 == 0) { g->v38bc = 3; g->flag38c0 = 1; }
                else {
                    g->v38bc = (int32_t)e->rnd(seed); g->flag38c0 = 0;
                    if (g->v91380 == 0) g->v91380 = 1;
                    else {
                        int a = (int)e->rnd(seed);
                        void* t = e->fn38a70(0, a, 0, 0);
                        if (t) { int b3 = (int)e->rnd(seed); int c = (int)e->rnd(seed); e->fn38ba0(t, c, b3, 0); }
                    }
                }
            }
            if (g->cnt2ac > 9) {
                int32_t w = g->cellW, q = ((w + ((w >> 31) & 3)) >> 2);
                if (g->mode414 == 1) i8 = w - q * 2;
                else { i8 = w; if (g->mode414 == 2) { e->ord10014(q << 1, 0, w, g->cellH - 0x2f, 0xff, 5, 0, f); return; } }
                e->ord10014(0, 0, i8, g->cellH - 0x2f, 0xff, 5, 0, f);
                return;
            }
        }
        if (i8 != 0 && rd<int32_t>(g->ctx, 0x114) != 0) e->fn2fe80();
        return;
    }
    e->fatal(0x573);
}

// 1.13c D2Client+0x80700
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_80700(D2cB8G* g, const D2cB8Env* e, uint64_t* seed) {
    uint32_t h = e->ord10064();
    if (h == 0xFFFFFFFFu) return;
    uint8_t* p = rd<uint8_t*>(g->ctx, 0x11c) + (int16_t)(h >> 16) * 0x28;
    const uint64_t M = 0x6ac690c5;
    auto step = [&]() { *seed = (uint64_t)(uint32_t)*seed * M + (uint32_t)(*seed >> 32); };
    wr<int32_t>(p, 0x1c, 0);
    int32_t div = g->cellH - 0x57;
    wr<int32_t>(p, 4, (int32_t)e->rnd(seed));
    wr<int32_t>(p, 0xc, (int32_t)e->rnd(seed));
    wr<int32_t>(p, 8, (int32_t)e->rnd(seed));
    step();
    wr<uint32_t>(p, 0x18, (uint32_t)*seed & 0x1ff);
    step();
    if (g->rainMode != 0) {
        wr<uint8_t>(p, 0x25, 0xff);
        wr<int32_t>(p, 0x20, 1);
        wr<int32_t>(p, 0x14, 8 - e->ftol());
        int32_t v = ((rd<int32_t>(p, 0xc) - 0x28) * 7) / div;
        if (g->flag7c) v++;
        wr<int32_t>(p, 0x10, v);
        if (v < 8 && v >= 0) {
            uint8_t b = 0, o = 0;
            d2_client_804c0(g, e, &o, &b, 0);   // ponytail: retail passes EBX/EDI in regs; caller context unrecovered
            wr<uint8_t>(p, 0x24, o);
            return;
        }
        e->fatal(0x390);
    }
    uint32_t i2 = (uint32_t)*seed % 12;
    uint32_t lv = e->ord11044(g->levelId, 0);
    switch (lv) {
    case 0: wr<uint8_t>(p, 0x24, g->tab38d4[i2]); wr<uint8_t>(p, 0x25, 0x7f); break;
    case 1: case 3: wr<uint8_t>(p, 0x24, g->tab38a0[i2]); wr<uint8_t>(p, 0x25, 0xff); break;
    case 2: wr<uint8_t>(p, 0x24, g->tab38f8[i2]); wr<uint8_t>(p, 0x25, 0xff); break;
    }
    wr<int32_t>(p, 0x10, 4 - e->ftol());
    wr<int32_t>(p, 0x14, 0xf - e->ftol());
    wr<int32_t>(p, 0x20, 3);
}

// 1.13c D2Client+0x80F60
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_80f60(D2cB8G* g, const D2cB8Env* e, uint64_t* seed) {
    if (g->rainMode) e->ord10083((uint32_t)g->trigHandle);
    uint8_t* p = rd<uint8_t*>(g->ctx, 0x11c);
    for (int32_t i = 0; i <= rd<int32_t>(g->ctx, 0x110); i++, p += 0x28) {
        if (rd<int16_t>(p, 0) == 0) continue;
        e->ftol();
        e->ord10083((uint32_t)g->trigHandle);
        int32_t d = e->ftol();
        e->ord10084((uint32_t)g->trigHandle);
        int32_t t = rd<int32_t>(p, 8) + (e->ftol() - g->tickDY);
        wr<int32_t>(p, 8, t);
        if (rd<int32_t>(p, 0xc) < t) { wr<int16_t>(p, 0x1c, 1); wr<int16_t>(p, 0x1e, 0); }
        if (rd<int32_t>(p, 0x1c) != 0) {
            if (rd<int32_t>(p, 0x20) == 0) {
                e->ord10065();
                if (rd<int32_t>(g->ctx, 0x114) < g->maxLive) d2_client_80700(g, e, seed);
            } else {
                d = 0; wr<int16_t>(p, 0x14, 0); wr<int16_t>(p, 0x16, 0);
                wr<int32_t>(p, 0x20, rd<int32_t>(p, 0x20) - 1);
            }
        }
        if (rd<int16_t>(p, 0) != 0) {
            if (g->rainMode && rd<int32_t>(p, 0x1c) == 0) {
                e->ord10084((((uint32_t)g->frame << 9) / 0x19 + rd<int32_t>(p, 0x18)) & 0x1ff);
                d += e->ftol();
            }
            int32_t w = g->cellW;
            d = (rd<int32_t>(p, 4) - g->tickDX) + d;
            wr<int32_t>(p, 4, d);
            if (w <= d) wr<int32_t>(p, 4, d - w);
            if (rd<int32_t>(p, 4) < 0) wr<int32_t>(p, 4, rd<int32_t>(p, 4) + w);
        }
    }
    g->frame++;
}

// 1.13c D2Client+0x81D70
extern "C" EMSCRIPTEN_KEEPALIVE int d2_client_81d70(D2cB8G* g, const D2cB8Env* e, const uint8_t* unit) {
    int r = 0;
    uint8_t* t = e->ord10695(unit ? rd<int32_t>(unit, 4) : -1);
    if (t && rd<int16_t>(t, 0x124) > 0) r = rd<int16_t>(t, 0x126);
    int k = e->ord10106();
    if (k == 7) {
        int i = e->ord10620();
        if (i >= 0 && i < rd<int32_t>(g->dt, 0xc28)) {
            uint8_t* rec = rd<uint8_t*>(g->dt, 0xc24) + (size_t)i * 0x14c;
            if (rec && rd<int16_t>(rec, 0x84) > 0) return rd<int16_t>(rec, 0x86);
        }
    } else if (e->ord10106() == 5) {
        int i = e->ord10620();
        if (i >= 0 && i < rd<int32_t>(g->dt, 0xc1c)) {
            uint8_t* rec = rd<uint8_t*>(g->dt, 0xc18) + (size_t)i * 0x1b8;
            if (rec && rd<int16_t>(rec, 0x82) > 0) r = rd<int16_t>(rec, 0x84);
        }
    }
    return r;
}

// 1.13c D2Client+0x81E40
extern "C" EMSCRIPTEN_KEEPALIVE int d2_client_81e40(D2cB8G* g, const D2cB8Env* e, const uint8_t* unit, uint32_t* out) {
    int r = 0;
    if (out) *out = 0xc;
    uint8_t* t = e->ord10695(unit ? rd<int32_t>(unit, 4) : -1);
    if (t && rd<int16_t>(t, 0x124) > 0) {
        if (out && t[0x128]) *out = t[0x128];
        r = rd<int16_t>(t, 0x124);
    }
    int k = e->ord10106();
    if (k == 7) {
        if (e->ord10620() >= 0) {
            int i = e->ord10620();
            if (i >= 0 && i < rd<int32_t>(g->dt, 0xc28)) {
                uint8_t* rec = rd<uint8_t*>(g->dt, 0xc24) + (size_t)i * 0x14c;
                if (rec && rd<int16_t>(rec, 0x84) > 0) {
                    if (out && rec[0x88]) *out = rec[0x88];
                    return rd<int16_t>(rec, 0x84);
                }
            }
        }
    } else if (e->ord10106() == 5) {
        int i = e->ord10620();
        if (i >= 0 && i < rd<int32_t>(g->dt, 0xc1c)) {
            uint8_t* rec = rd<uint8_t*>(g->dt, 0xc18) + (size_t)i * 0x1b8;
            if (rec && rd<int16_t>(rec, 0x82) > 0) {
                if (out && rec[0x86]) *out = rec[0x86];
                r = rd<int16_t>(rec, 0x82);
            }
        }
    }
    return r;
}

// 1.13c D2Client+0x81F60
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_81f60(D2cB8G* g, const D2cB8Env* e, const uint8_t* unit) {
    int r = e->ord10787();
    if (r == 0) r = (unit == g->player && g->s97f8 < g->s985c) ? 8 : 0;
    return (r != 2 && r != 7);
}

// 1.13c D2Client+0x81FF0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_81ff0(D2cB8G* g, const D2cB8Env* e, const uint8_t* unit, const uint32_t* a) {
    (void)g;
    if (!a) return;
    int32_t uid = unit ? rd<int32_t>(unit, 4) : -1;
    auto chk = [&](uint32_t c0, uint32_t c1, uint32_t c2) {
        if (a[0] != 4) e->fatal(c0);
        uint8_t* t = e->ord10695(uid);
        if (!t) e->fatal(c1);
        if (!e->ord10806(rd<uint32_t>(t, 0xf0))) e->fatal(c2);
        e->ord10519();
    };
    if (e->ord10744() == 0) {
        if (e->ord10744() == 0) e->ord10600();
        else { chk(0x3a, 0x3c, 0x3e); e->ord10996a(5); }
    } else { chk(0x30, 0x32, 0x34); e->ord10996b(2, 0); }
    if (e->ord10202() != 0) return;
    if (a[0] == 0) {
        uint32_t o = e->ord10061(a[0x18]);
        if (e->ord10747() != 0) {
            if (o && (uint32_t)(uintptr_t)unit != o) e->ord10744();
            e->ord10202(); e->ord10164();
            return;
        }
    }
    e->ord10202(); e->ord10164();
}

// 1.13c D2Client+0x82210
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_82210(D2cB8G* g, const D2cB8Env* e, uint8_t* u, int32_t p2, uint32_t p3, uint32_t esi) {
    int32_t k = (p2 << (rd<uint32_t>(g->dt, 0xc6c) & 0x1f)) + (int32_t)(rd<uint32_t>(g->dt, 0xc70) & p3);
    uint8_t* it = e->ord10195(u, 0x40);
    int32_t r;
    for (;;) {
        if (!it) {
            if (rd<int32_t>(u, 0x60)) {
                for (uint8_t* c = e->ord10460(rd<uint32_t>(u, 0x60)); c; c = e->ord10464(c))
                    if (d2_client_82210(g, e, e->ord11147(c), p2, esi, esi)) return 1;
            }
            return 0;
        }
        r = e->ord10680(it, 0xcc, k);
        if (r) break;
        it = e->ord10900(it, 0x40);
    }
    int32_t v = r >> 8;
    if (v < 1) return 0;
    if (v < 0x100) {
        int32_t s = ((int32_t)esi < 1) ? 0 : (int32_t)esi;
        if (v <= s) s = v;
        e->ord11037(it, 0xcc, v * 0x100 + (s & 0xff), k, u);
        return 1;
    }
    return 0;
}

// 1.13c D2Client+0x82320
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_82320(D2cB8G* g, const D2cB8Env* e, const uint8_t* pkt) {
    uint32_t key = rd<uint32_t>(pkt, 4);
    uint8_t* n = rd<uint8_t*>(g->hashA, (key & 0x7f) * sizeof(uint8_t*));
    while (n) {
        if (rd<uint32_t>(n, 0xc) == key) { if (rd<int32_t>(n, 0) != 4) e->fatal(0x5a); break; }
        n = rd<uint8_t*>(n, 0xe4);
    }
    int32_t ok;
    e->ord11145(n, (uint8_t*)pkt + 8, pkt[2] - 8, 0, 0, 0x60, &ok);
    if (ok) { e->fn56ef0(g->hashA, key & 0x7f, key, 4); return; }
    if (!g->player) e->fatal(0x7b3);
    uint32_t l = rd<uint32_t>(g->player, 0x60);
    if (!l) e->fatal(0x7b6);
    uint32_t h = e->ord10838(n, 8);
    if (e->ord10197(h) == 0) { e->ord10466(l, 0); e->ord11017(l); e->fn62d0(); }
    if (e->ord11017(l) != 0) e->fn31aa0();
}

// 1.13c D2Client+0x82410
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_82410(D2cB8G* g, const D2cB8Env* e, const uint8_t* s) {
    if (rd<int32_t>(s, 0x10)) {
        int a = e->ord10708(rd<uint32_t>(s, 0x18));
        if (a && a != e->ord10828() && e->fn81f60((uint8_t*)s)) e->ord10546();
    }
    if (rd<int32_t>(s, 0xc)) {
        int a = e->ord10708(rd<uint32_t>(s, 0x14));
        if (a && a != e->ord10507() && e->fn81f60((uint8_t*)s)) e->ord10978();
    }
    (void)g;
}

// 1.13c D2Client+0x82490
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_82490(D2cB8G* g, const D2cB8Env* e, uint8_t* s, const int32_t* bp, uint32_t ret) {
    uint32_t id = rd<uint32_t>(s, 0x60);
    char c = e->ord10417();
    if (c == 4 || c == 5) {
        e->ord10694(id, ret); e->ord10921(); e->fn19a90(s); e->ord10605();
    }
    if (e->ord10646(id, ret)) {
        if (rd<int32_t>(s, 0) == 0) e->ord10061(rd<uint32_t>(s, 0x60));
        if (e->ord10202b(0x100, 0x96, 0, "..\\Source\\D2Client\\UNIT\\Item.cpp") == 0) e->ord11127();
        e->ord10608(id, c);
    }
    int32_t t = 6; uint32_t k = 0xffffffffu;
    if (bp) { t = bp[0]; k = (uint32_t)bp[3]; }
    e->fn56ef0(g->hashB + (size_t)t * 0x80, k & 0x7f, k, t);
}
