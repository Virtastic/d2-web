// src/core/d2client/d2client_batch_b4fb0.cpp -- 12 UNMAPPED 1.13c D2Client rows (0xB4FB0..0xB6120) from the
// cached decompiles, alpha lane: implementation + marker, no spec/vector. These are the missile/monster
// "spawn effect" message builders: each zeroes a 0x5C-byte record (23 dwords), fills it, and posts it
// through FUN_6fb630f0 (Env::post, returns the new unit/gfx object or 0). Unit/record memory is retail
// 32-bit layout, reached through Env rd32/rd16/wr32 (addresses are uintptr_t). Register args (EAX, EBX,
// ESI, EDI) are explicit params. Ordinal_* callees are Env seams named by ordinal.
#include <cstdint>
#include <cstddef>
#include <initializer_list>
#include <emscripten.h>

struct B4fb0Rec { uint32_t w[23]; }; // w[k] = dword at -0x5C+4k of the retail frame

struct B4fb0Env {
    uint32_t  (*rd32)(uintptr_t);
    int32_t   (*rd16s)(uintptr_t);
    void      (*wr32)(uintptr_t, uint32_t);
    uintptr_t (*dataTables)(void);                       // *sgptDataTables_exref
    uintptr_t (*post)(B4fb0Rec*);                        // FUN_6fb630f0
    uintptr_t (*owner)(uintptr_t);                       // FUN_6fb560d0
    uintptr_t (*findUnit)(uint32_t);                     // DAT_6fbbaa08 hash chain (id&0x7f, next +0xE4)
    void      (*fatal)(void);                            // Ordinal_10265/10024 + _exit(-1)
    uint32_t  (*o10240)(uintptr_t);   uint32_t (*o11029)(uintptr_t);
    void      (*o10637)(uintptr_t, uint32_t); void (*o10019)(uintptr_t, uint32_t);
    uint32_t  (*o10867)(uintptr_t);   uint32_t (*o10750)(uintptr_t);
    uint32_t  (*o10042)(uint32_t);    void (*o10706)(uintptr_t, uint32_t);
    uint32_t  (*o10983)(uintptr_t);   uint32_t (*o10764)(uintptr_t);
    int32_t   (*o10080)(uintptr_t, int32_t, int32_t);
    int32_t   (*o11018)(uintptr_t);   int32_t (*o10786)(uint32_t, uint32_t, int32_t, uint32_t);
    void      (*f77b00)(void);        void (*f77c80)(uint32_t, uint32_t);
    uint32_t  (*f1330)(uintptr_t);    uint32_t (*f1630)(uintptr_t); uint32_t (*f1660)(uintptr_t);
    const int32_t *tab3770, *tab3728;                    // DAT_6fb83770 / DAT_6fb83728 (18 each)
    const uint32_t *tab37b8, *tab38b8, *tab39b8, *tab3ab8; // 64 dwords each
};
static B4fb0Env g_b4fb0;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b4fb0_set_env(const B4fb0Env* e) { g_b4fb0 = *e; }
#define E g_b4fb0

// Unit seed: 64-bit LCG at +0x20 (lo) / +0x24 (hi): seed = lo*0x6AC690C5 + hi.
static uint32_t lcg(uintptr_t u) {
    uint64_t s = (uint64_t)E.rd32(u + 0x20) * 0x6AC690C5u + E.rd32(u + 0x24);
    E.wr32(u + 0x20, (uint32_t)s); E.wr32(u + 0x24, (uint32_t)(s >> 32));
    return (uint32_t)s;
}
// Range roll (0..m-1): m<1 -> 0 without advancing; power of two -> mask; else modulo.
static uint32_t roll(uintptr_t u, uint32_t m) {
    if ((int32_t)m < 1) return 0;
    uint32_t s = lcg(u);
    return (m & (m - 1)) == 0 ? (m - 1) & s : s % m;
}
static bool ownerPos(uint32_t t) { return t == 2 || (t > 3 && t < 6); }
static int32_t unitX(uintptr_t u) {
    uint32_t t = E.rd32(u), p = E.rd32(u + 0x2c);
    return ownerPos(t) ? (int32_t)E.rd32(p + 0xc) : p ? (int32_t)E.o10867(p) : 0;
}
static int32_t unitY(uintptr_t u) {
    uint32_t t = E.rd32(u), p = E.rd32(u + 0x2c);
    return ownerPos(t) ? (int32_t)E.rd32(p + 0x10) : p ? (int32_t)E.o10750(p) : 0;
}
// Monster/object record: dt+0xB64 base, dt+0xB6C count, stride 0x1A4; 0 when unit/idx invalid.
static uintptr_t unitRec(uintptr_t u) {
    if (!u) return 0;
    int32_t i = (int32_t)E.rd32(u + 4); uintptr_t dt = E.dataTables();
    if (i < 0 || i >= (int32_t)E.rd32(dt + 0xb6c)) return 0;
    return E.rd32(dt + 0xb64) + (uintptr_t)i * 0x1a4;
}
// Zero the record's gfx sub-block fields at (gfxObj+0x54)->+0x30 + off (the three guarded clears).
static void gfxClear(uintptr_t g, uint32_t off, uint32_t a, uint32_t b, uint32_t c) {
    uintptr_t p = E.rd32(g + 0x54); if (!p) return;
    uintptr_t q = E.rd32(p + 0x30); if (!q) return;
    E.wr32(q + off, a); E.wr32(q + off + 4, b); E.wr32(q + off + 8, c);
}

// 1.13c D2Client+0xB4FB0: 14 spawn records (type 5), each given a random 10..44 value. EBX = unit.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b4fb0(uintptr_t unit) {
    B4fb0Rec r{}; r.w[0] = 5; r.w[1] = 0xfffffffd;
    if (!E.owner(5)) return;
    for (int i = 2; i < 23; i++) r.w[i] = 0;
    r.w[2] = 0x8102;
    E.o10240(0); E.o11029(0);
    for (int i = 0; i < 14; i++) {
        uintptr_t p = E.post(&r);
        if (!p) continue;
        E.o10637(p, r.w[22]);
        uint64_t s = (uint64_t)E.rd32(unit + 0x20) * 0x6AC690C5u + E.rd32(unit + 0x24);
        E.wr32(unit + 0x20, (uint32_t)s); E.wr32(unit + 0x24, (uint32_t)(s >> 32));
        E.o10019(p, (uint32_t)s % 0x23 + 10);
    }
}

// 1.13c D2Client+0xB5380: EAX = target unit id, EBX = source unit, param_1 = message type.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b5380(uint32_t id, uintptr_t src, uint32_t msg, uint32_t edi) {
    uintptr_t t = E.findUnit(id);
    if (!t) return;
    if (E.rd32(t) != 2) { E.fatal(); return; }
    for (uintptr_t u : {src, t}) {
        uint32_t ty = E.rd32(u);
        if (ty != 2 && (ty < 4 || ty > 5) && E.rd32(u + 0x2c)) E.o10867(E.rd32(u + 0x2c));
    }
    int32_t a = unitY(src), b = unitY(t);
    B4fb0Rec r{}; r.w[1] = msg;
    E.o10240(0); E.o11029(0);
    uintptr_t p = E.post(&r);
    if (!p) return;
    E.f77b00();
    gfxClear(p, 4, 0, 0, 0);
    gfxClear(p, 0x10, 0, 0, 0x1000);
    gfxClear(p, 0x1c, 0, 0, 0);
    E.o10637(p, edi);
    E.o10019(p, (uint32_t)(a - b));
}

// 1.13c D2Client+0xB55C0: EAX = n, ESI = unit. Posts type 0x21 at a random offset in +-n of the unit.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b55c0(int32_t n, uintptr_t unit) {
    uint32_t m = (uint32_t)n * 2;
    uint32_t rx = roll(unit, m), ry = roll(unit, m);
    B4fb0Rec r{}; r.w[0] = 0x21; r.w[1] = (uint32_t)E.owner(0);
    r.w[5] = (uint32_t)unitX(unit); r.w[6] = (uint32_t)unitY(unit);
    r.w[7] = (uint32_t)(unitX(unit) + (int32_t)(rx - n));
    r.w[8] = (uint32_t)(unitY(unit) + (int32_t)(ry - n));
    E.o10240(0); r.w[10] = E.o11029(0);
    r.w[2] = 0; // uStack_4: caller-frame slot, unrecovered
    E.post(&r);
}

// 1.13c D2Client+0xB5770: __fastcall(ECX = unit). Posts two records from rec+0x2C / +0x2E.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_b5770(uintptr_t unit) {
    uintptr_t rec = unitRec(unit);
    if (!rec) return 1;
    B4fb0Rec r{}; r.w[1] = (uint32_t)E.owner(0);
    E.o10240(unit); E.o11029(unit);
    if (E.rd16s(rec + 0x2c) >= 0) { r.w[2] = (uint32_t)E.rd16s(rec + 0x2c); E.post(&r); }
    if (E.rd16s(rec + 0x2e) >= 0) {
        r.w[2] = (uint32_t)E.rd16s(rec + 0x2e);
        uintptr_t p = E.post(&r);
        if (p) gfxClear(p, 4, 0, 0, E.rd32(rec + 0x6c) << 11);
    }
    return 1;
}

// 1.13c D2Client+0xB5850: __fastcall(ECX = unit). One record from rec+0x2C, then ord10706(p+0x2C, ord10042).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_b5850(uintptr_t unit) {
    uintptr_t rec = unitRec(unit);
    if (!rec || E.rd16s(rec + 0x2c) < 0) return 1;
    if (!E.owner(E.dataTables())) return 1;
    B4fb0Rec r{}; r.w[4] = (uint32_t)E.rd16s(rec + 0x2c);
    E.o10240(unit); E.o11029(unit); r.w[0] = (uint32_t)unit;
    uintptr_t p = E.post(&r);
    if (p) E.o10706(E.rd32(p + 0x2c), E.o10042(E.rd32(unit + 0x2c)));
    return 1;
}

// 1.13c D2Client+0xB5930: __fastcall(ECX = unit). Posts records for rec+0x2C, +0x2E, +0x30 (when >= 0).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_b5930(uintptr_t unit) {
    uintptr_t rec = unitRec(unit);
    if (!rec) return 1;
    uintptr_t o = E.owner(E.dataTables());
    if (!o) return 1;
    B4fb0Rec r{}; r.w[1] = (uint32_t)o;
    E.o10240(unit); E.o11029(unit);
    for (uint32_t off : {0x2cu, 0x2eu, 0x30u})
        if (E.rd16s(rec + off) >= 0) { r.w[2] = (uint32_t)E.rd16s(rec + off); E.post(&r); }
    return 1;
}

// 1.13c D2Client+0xB5A10: __fastcall(ECX = unit). Type 0x21 record at the unit's position.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_b5a10(uintptr_t unit) {
    uintptr_t rec = unitRec(unit);
    if (!rec || E.rd16s(rec + 0x2c) < 0) return 1;
    B4fb0Rec r{}; r.w[0] = 0x21; r.w[1] = (uint32_t)E.owner(0);
    r.w[5] = (uint32_t)unitX(unit); r.w[6] = (uint32_t)unitY(unit);
    E.o10240(unit); r.w[10] = E.o11029(unit);
    r.w[2] = (uint32_t)E.rd16s(rec + 0x2c);
    uintptr_t h = E.rd32(unit + 0x2c);
    r.w[3] = E.o10983(h); r.w[3] = E.o10764(h); // both land in the same slot (uStack_4c)
    uintptr_t p = E.post(&r);
    if (p) {
        uintptr_t g = E.rd32(p + 0x54), q = g ? E.rd32(g + 0x30) : 0;
        if (q) { E.wr32(q, E.rd32(q) | 4); E.wr32(q + 0x40, E.rd32(rec + 0x6c)); E.wr32(q + 0x44, E.rd32(rec + 0x70)); }
        E.f77c80(E.rd32(rec + 0x74), 0);
        g = E.rd32(p + 0x54); q = g ? E.rd32(g + 0x30) : 0;
        if (q) E.wr32(q, E.rd32(q) & 0xfffffffd);
    }
    return 1;
}

// 1.13c D2Client+0xB5B70: __thiscall(ECX = this unit, EAX = n, stack: p1, p2, p3).
// p2 >= 0: n type-0x524 scatter records; p3 >= 0: 18 type-1 ring records from tab3770/tab3728.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b5b70(uintptr_t self, int32_t n, uint32_t p1, int32_t p2, int32_t p3) {
    if (!E.owner(self)) return;
    E.o10240(self); uint32_t o = E.o11029(self);
    int32_t x = unitX(self), y = unitY(self);
    if (p2 >= 0) {
        B4fb0Rec r{}; r.w[0] = 0x524; r.w[4] = 0x1c8; r.w[2] = (uint32_t)self; r.w[10] = p1;
        for (int32_t i = n; i > 0; i--) {
            uint32_t m = (uint32_t)n * 4 + 1;
            int32_t px = (int32_t)roll(self, m) - n * 2 + x;
            int32_t py = (int32_t)roll(self, m) - n * 2 + y;
            if (E.o10080(self, px, py) > 3) { r.w[6] = (uint32_t)px; r.w[7] = (uint32_t)py; E.post(&r); }
        }
    }
    if (p3 >= 0) {
        for (int i = 0; i < 18; i++) {
            B4fb0Rec r{}; r.w[0] = 1; r.w[4] = (uint32_t)p3;
            r.w[5] = (uint32_t)(E.tab3770[i] + x); r.w[6] = (uint32_t)(E.tab3728[i] + y); r.w[22] = o;
            E.post(&r);
        }
    }
}

// 1.13c D2Client+0xB5DD0: __fastcall(ECX = unit). Type 0x102 records, one per ord11018 count,
// with a random 1..5 and random offsets about +-max(rec+0x6C,3).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_b5dd0(uintptr_t unit) {
    uintptr_t rec = unitRec(unit);
    if (!rec || E.rd16s(rec + 0x2c) < 0) return 1;
    uintptr_t o = E.owner(E.dataTables()); if (!o) return 1;
    B4fb0Rec r{}; r.w[1] = (uint32_t)o; r.w[4] = (uint32_t)E.rd16s(rec + 0x2c);
    r.w[0] = 0x102; r.w[2] = (uint32_t)unit; r.w[10] = E.o10240(unit);
    int32_t k = (int32_t)E.rd32(rec + 0x6c); if (k < 3) k = 3;
    for (int32_t n = E.o11018(unit); n > 0; n--) {
        r.w[10] = lcg(unit) % 5 + 1;
        uint32_t a = E.f1330(unit + 0x20), b = E.f1330(unit + 0x20);
        r.w[5] = a - k; r.w[6] = b - k;
        if (r.w[5] == 0 && r.w[6] == 0) r.w[5] = E.f1330(unit + 0x20) + 1;
        E.post(&r);
    }
    return 1;
}

static void ring(uintptr_t unit, const uint32_t* tA, const uint32_t* tB) {
    uintptr_t rec = unitRec(unit);
    if (!rec || E.rd16s(rec + 0x2c) < 0) return;
    uintptr_t o = E.owner(E.dataTables()); if (!o) return;
    B4fb0Rec r{}; r.w[1] = (uint32_t)o; r.w[0] = 2; r.w[2] = (uint32_t)unit;
    E.o10240(unit); r.w[10] = E.o11029(unit);
    r.w[2] = (uint32_t)E.rd16s(rec + 0x2c);
    int32_t step = (int32_t)E.rd32(rec + 0x6c); if (step < 1) step = 1;
    for (int32_t i = 0; i < 0x100; i += step * 4) {
        r.w[6] = tA[i / 4]; r.w[5] = tB[i / 4];
        uintptr_t p = E.post(&r);
        if (p) { E.o10637(p, tB[i / 4]); E.o10019(p, tA[i / 4]); }
    }
}
// 1.13c D2Client+0xB5F00: __fastcall(ECX = unit). Ring from tab37b8 (y) / tab38b8 (x).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_b5f00(uintptr_t unit) { ring(unit, E.tab37b8, E.tab38b8); return 1; }
// 1.13c D2Client+0xB6010: same, tables tab39b8 / tab3ab8.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_b6010(uintptr_t unit) { ring(unit, E.tab39b8, E.tab3ab8); return 1; }

// 1.13c D2Client+0xB6120: __fastcall(ECX = unit). Type 1 record; duration rec+0x6C (0 -> ord10786, min 5).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_b6120(uintptr_t unit, uint32_t esi) {
    uintptr_t rec = unitRec(unit);
    if (!rec || E.rd16s(rec + 0x2c) < 0 || !E.owner(E.dataTables())) return 1;
    int32_t idx = (int32_t)E.o10240(unit); uint32_t v = E.o11029(unit);
    uintptr_t dt = E.dataTables();
    if (idx < 0 || idx >= (int32_t)E.rd32(dt + 0xba0)) return 1;
    uintptr_t m = E.rd32(dt + 0xb98) + (uintptr_t)idx * 0x23c;
    int32_t dur = (int32_t)E.rd32(rec + 0x6c);
    if (dur == 0) { dur = E.o10786(esi, E.rd32(m + 0x138), idx, v); if (dur < 5) dur = 5; }
    B4fb0Rec r{}; r.w[0] = 1; r.w[4] = (uint32_t)E.rd16s(rec + 0x2c);
    r.w[10] = v; r.w[11] = v; r.w[5] = E.f1630(unit); r.w[6] = E.f1660(unit);
    if (dur > 0) { r.w[0] = 0x8001; r.w[22] = (uint32_t)dur; }
    uintptr_t p = E.post(&r);
    if (p) E.o10706(E.rd32(p + 0x2c), E.o10042(E.rd32(unit + 0x2c)));
    return 1;
}
