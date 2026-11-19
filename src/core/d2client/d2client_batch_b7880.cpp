// src/core/d2client/d2client_batch_b7880.cpp -- twelve UNMAPPED 1.13c D2Client.dll rows (0xB7880..0xB8960),
// reimplemented from build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane). Per-tick "unit update"
// handlers for the client-side effect units: each validates the record sgptDataTables+0xB64 + class*0x1A4
// (count at +0xB6C), then builds a zeroed spawn-descriptor and hands it to 0xB30F0.
// Env-seam style: unrecovered callees (Ordinal_*, 0xB30F0, 0xA60D0 ...) are Env function pointers;
// unit/record memory goes through rd/wr seams that take the RETAIL offset (the host widens it for
// wasm64). Descriptor words are uintptr_t (pointer fields widen); word index = (retail stack offset
// from the pointer passed to 0xB30F0) / 4. Hidden register inputs (EBP/ESI/EAX leftovers) are explicit
// params. Decompile stack aliasing makes the word placement of 10240/11029/x/y a GUESS where noted.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2ClientBatchB7880Env {
    uintptr_t (*rec_b7880)(int32_t cls);                       // class record or 0 (range check, +0xB64/+0xB6C)
    int32_t   (*rd32_b7880)(uintptr_t p, uint32_t off);        // retail offsets
    int32_t   (*rd16s_b7880)(uintptr_t p, uint32_t off);
    uint32_t  (*rd8_b7880)(uintptr_t p, uint32_t off);
    uintptr_t (*rdp_b7880)(uintptr_t p, uint32_t off);
    void      (*wr32_b7880)(uintptr_t p, uint32_t off, int32_t v);
    uint64_t* (*seed_b7880)(uintptr_t unit);                   // unit +0x20 (lo) / +0x24 (hi)
    int32_t   (*tbl32_b7880)(uint32_t rva, uint32_t idx);      // image table at D2Client+rva
    int32_t   (*x_b7880)(uintptr_t u);                         // 0xA1630
    int32_t   (*y_b7880)(uintptr_t u);                         // 0xA1660
    uintptr_t (*f560d0_b7880)(uintptr_t a);                    // 0xA60D0
    uintptr_t (*spawn_b7880)(uintptr_t* desc);                 // 0xB30F0 (returns unit or 0)
    void      (*done_b7880)(uintptr_t u);                      // 0xB17C0
    void      (*bad_b7880)();                                  // 0xB1720
    void      (*fallback_b7880)(uint32_t rva, uint32_t a, uint32_t b, int32_t c);  // 0xA6EF0
    uint32_t  (*o10985_b7880)(uintptr_t u);
    int32_t   (*o10983_b7880)(uintptr_t path);
    int32_t   (*o10764_b7880)(uintptr_t path);
    void      (*o10637_b7880)(uintptr_t u, int32_t v);
    void      (*o10019_b7880)(uintptr_t u, int32_t v);
    int32_t   (*o10203_b7880)(uintptr_t path);
    int32_t   (*o10240_b7880)(uintptr_t u);
    int32_t   (*o11029_b7880)(uintptr_t u);
    int32_t   (*o11018_b7880)(uintptr_t u);
    int32_t   (*o10818_b7880)(uintptr_t u);
    int32_t   (*o10331_b7880)(uintptr_t u);
    int32_t   (*o10851_b7880)(uintptr_t room, int32_t x, int32_t y, int32_t a, int32_t b);
    uintptr_t (*room_b7880)();                                 // 0xA1400 (hidden-register room lookup)
    void      (*o10286_b7880)(uintptr_t path, int32_t x, int32_t y);
    void      (*o10334_b7880)(uintptr_t path, uintptr_t u, int32_t a);
    int32_t   (*o10042_b7880)(uintptr_t path);
    void      (*o10706_b7880)(uintptr_t path, int32_t v);
    int32_t   (*o11080_b7880)(uintptr_t u);
    void      (*o10236_b7880)(uintptr_t path, int32_t v);
    int32_t   (*o11168_b7880)(uintptr_t u);
    void      (*o10575_b7880)(uintptr_t path, int32_t v);
    void      (*f77b00_b7880)();                               // 0xC7B00
    void      (*f77e30_b7880)(int32_t a, int32_t b, int32_t c);// 0xC7E30
    int32_t   (*f77ec0_b7880)();                               // 0xC7EC0
    int32_t   (*f60050_b7880)();                               // 0xB0050
};

namespace {
typedef D2ClientBatchB7880Env E;
struct Desc { uintptr_t w[25]; Desc() { for (auto& x : w) x = 0; } };
const uint32_t kPath = 0x2C;   // unit pPath (param_1[0xb])

// Seeded rand(n) shared by 0xA1330/0xA1380 (n arrives in a register).
uint32_t rnd(const E* e, uintptr_t u, uint32_t n) {
    if ((int32_t)n < 1) return 0;
    uint64_t* s = e->seed_b7880(u);
    uint64_t v = (uint64_t)(uint32_t)*s * 0x6ac690c5ull + (uint32_t)(*s >> 32);
    *s = v;
    return (n & (n - 1)) == 0 ? ((n - 1) & (uint32_t)v) : (uint32_t)((v & 0xffffffffu) % n);
}
uintptr_t validRec(const E* e, uintptr_t u) {
    if (!u) return 0;
    return e->rec_b7880(e->rd32_b7880(u, 4));
}
int32_t cls1e(const E* e, uintptr_t r) { return e->rd16s_b7880(r, 0x1E); }
int32_t adj(int32_t t) { return t + ((t >> 31) & 3); }
}  // namespace

// 1.13c D2Client+0xB7880 (361 B): ground-effect step. GUESS: the two 0xB30F0 dispatches share one
// descriptor (decompile shows two overlapping stack bases); w[10]/w[12] placement guessed. ebp = hidden.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b7880(uintptr_t u, int32_t ebp, const E* e) {
    uintptr_t r = 0;
    if (!u) { e->fallback_b7880(0x109A08 + 6 * 0x200, 0xffffffffu & 0x7f, 0xffffffffu, 6); return; }
    r = e->rec_b7880(e->rd32_b7880(u, 4));
    uintptr_t path = e->rdp_b7880(u, kPath);
    if (!(r && cls1e(e, r) >= 0 && path)) {
        int32_t t = e->rd32_b7880(u, 0); uint32_t c = (uint32_t)e->rd32_b7880(u, 0xC);
        e->fallback_b7880(0x109A08 + t * 0x200, c & 0x7f, c, t); return;
    }
    int32_t x = e->x_b7880(u); e->y_b7880(u);
    if ((int32_t)e->o10985_b7880(u) < 2) {
        int32_t a = e->o10983_b7880(path), b = e->o10764_b7880(path);
        e->o10637_b7880(u, -(b - ebp));
        e->o10019_b7880(u, a - x);
    }
    if (e->o10203_b7880(path)) {
        Desc d; Desc d2;
        d.w[1] = e->f560d0_b7880(0);
        d.w[10] = (uintptr_t)e->o10240_b7880(u); d.w[12] = (uintptr_t)e->o11029_b7880(u);
        e->o11018_b7880(u); e->o10818_b7880(u);
        e->spawn_b7880(d.w);
        e->o11018_b7880(u); e->o10818_b7880(u);
        e->spawn_b7880(d2.w);
    }
    e->done_b7880(u);
}

// 1.13c D2Client+0xB79F0 (330 B): frame-gated double spawn. esi = hidden rand range.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b79f0(uintptr_t u, uint32_t esi, const E* e) {
    uintptr_t r = validRec(e, u);
    if (!r || cls1e(e, r) < 0) { e->bad_b7880(); return; }
    int32_t n = (int32_t)e->o10985_b7880(u);
    if (n == e->rd32_b7880(r, 0x58)) e->o10019_b7880(u, 1);
    if (e->o10818_b7880(u)) {
        int32_t d = e->rd32_b7880(r, 0x5C) > 0 ? e->rd32_b7880(r, 0x5C) : 1;
        if (n % d == 0) {
            rnd(e, u, esi); rnd(e, u, esi);
            Desc dd; e->f560d0_b7880(0);
            e->x_b7880(u); e->y_b7880(u); e->x_b7880(u); e->y_b7880(u);
            dd.w[10] = (uintptr_t)e->o10240_b7880(u); dd.w[12] = (uintptr_t)e->o11029_b7880(u);
            e->spawn_b7880(dd.w);
        }
    }
    e->done_b7880(u);
}

// 1.13c D2Client+0xB7B40 (668 B, caller 0xBB570): scatter-spawn within radius. eax = class,
// ret = radius (stack leftover, named r), esi = rand range for the class bump. GUESS: iStack_4 is rec.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b7b40(int32_t eax, int32_t param1, uintptr_t unit, int32_t r, uint32_t esi, const E* e) {
    uintptr_t rec = e->rec_b7880(eax);
    if (!rec || !e->o10331_b7880(unit)) return;
    if (eax < cls1e(e, rec)) {
        eax += (int32_t)rnd(e, unit, esi);
        if (!e->rec_b7880(eax)) return;
    }
    (void)param1;
    Desc d;
    d.w[2] = 1; d.w[6] = (uintptr_t)eax;
    d.w[12] = (uintptr_t)e->o10240_b7880(unit); d.w[14] = (uintptr_t)e->o11029_b7880(unit);
    int32_t r2 = r * r; uint32_t m = (uint32_t)r * 2 + 1;
    for (int it = 1;; ++it) {
        int32_t dx = (int32_t)rnd(e, unit, m) - r;
        int32_t dy = (int32_t)rnd(e, unit, m) - r;
        if (dy * dy + dx * dx <= r2) {
            int32_t bx = e->x_b7880(unit), by = e->y_b7880(unit);
            e->o10331_b7880(unit);
            uintptr_t room = e->room_b7880();
            if (room && e->o10851_b7880(room, dx + bx, dy + by, 0, 1) == 0) {
                d.w[5] = (uintptr_t)(dx + bx); d.w[6] = (uintptr_t)(dy + by);
                uintptr_t p = e->spawn_b7880(d.w);
                if (p) {
                    e->f77b00_b7880();
                    int32_t k;
                    if (r == 6) { e->f77e30_b7880(e->rd32_b7880(rec, 0x58) << 1, 0, 0); k = e->rd32_b7880(rec, 0x5C) * -2; }
                    else        { e->f77e30_b7880(e->rd32_b7880(rec, 0x58), 0, 0);      k = -e->rd32_b7880(rec, 0x5C); }
                    uintptr_t pp = e->rdp_b7880(p, 0x54);
                    if (!pp) return;
                    uintptr_t q = e->rdp_b7880(pp, 0x30);
                    if (!q) return;
                    e->wr32_b7880(q, 0x10, 0); e->wr32_b7880(q, 0x14, 0); e->wr32_b7880(q, 0x18, k << 11);
                    return;
                }
            }
        }
        if (it > 3) return;
    }
}

// 1.13c D2Client+0xB7DE0 (553 B, caller 0xBB5B0): burst of up to 8 spawns with random placement.
// eax = count, param1 = descriptor w[1], cls = class, unit = ESI (owner whose seed is used).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b7de0(int32_t eax, uintptr_t param1, int32_t cls, uintptr_t unit, const E* e) {
    uintptr_t rec = e->rec_b7880(cls);
    if (!rec) return;
    int32_t n = eax * 7;
    int32_t cnt = n < 9 ? n : 8;
    for (; cnt > 0; --cnt) {
        Desc d; d.w[1] = param1;
        d.w[12] = (uintptr_t)e->o10240_b7880(unit); d.w[14] = (uintptr_t)e->o11029_b7880(unit);
        uintptr_t p = e->spawn_b7880(d.w);
        if (!p) continue;
        uint32_t ux = rnd(e, unit, (uint32_t)n), uy = rnd(e, unit, (uint32_t)n);
        uint32_t k = (uint32_t)e->rd32_b7880(rec, 0x5C);
        k = (int32_t)k < 1 ? 0 : rnd(e, unit, k);
        e->f77b00_b7880();
        int32_t v = e->rd32_b7880(rec, 0x58);
        uintptr_t pp = e->rdp_b7880(p, 0x54);
        uintptr_t q = pp ? e->rdp_b7880(pp, 0x30) : 0;
        if (q) {
            e->wr32_b7880(q, 4, (int32_t)(ux - n / 2) * 0x800);
            e->wr32_b7880(q, 8, (int32_t)(uy - n / 2) * 0x800);
            e->wr32_b7880(q, 0xC, v << 11);
        }
        if (q) {
            e->wr32_b7880(q, 0x10, 0); e->wr32_b7880(q, 0x14, 0); e->wr32_b7880(q, 0x18, (int32_t)k << 11);
        }
        int32_t v2 = e->rd32_b7880(rec, 0x60);
        if (q) {
            e->wr32_b7880(q, 0x1C, 0); e->wr32_b7880(q, 0x20, 0); e->wr32_b7880(q, 0x24, v2 << 11);
        }
    }
}

// 1.13c D2Client+0xB8010 (0x120 B): four-corner spawn at frame == rec+0x58. ebp/esi = hidden
// offsets added to tables at D2Client+0xB3DF0 (x) / +0xB3E00 (y), four int32 each.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b8010(uintptr_t u, int32_t ebp, int32_t esi, const E* e) {
    uintptr_t r = validRec(e, u);
    if (!r || cls1e(e, r) < 0) { e->bad_b7880(); return; }
    uintptr_t p = e->f560d0_b7880(0);
    if (p && (int32_t)e->o10985_b7880(u) == e->rd32_b7880(r, 0x58)) {
        Desc d;
        d.w[0] = (uintptr_t)e->x_b7880(p); d.w[1] = (uintptr_t)e->y_b7880(p);
        d.w[2] = 0x2001; d.w[3] = p; d.w[4] = (uintptr_t)cls1e(e, r);
        d.w[12] = (uintptr_t)e->o10240_b7880(u); d.w[14] = (uintptr_t)e->o11029_b7880(u);
        for (uint32_t i = 0; i < 4; ++i) {
            d.w[6] = (uintptr_t)(e->tbl32_b7880(0xB3DF0, i) + ebp);
            d.w[5] = (uintptr_t)(e->tbl32_b7880(0xB3E00, i) + esi);
            e->spawn_b7880(d.w);
        }
    }
    e->done_b7880(u);
}

// 1.13c D2Client+0xB8130 (381 B): spawn at start + random-jitter drift.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b8130(uintptr_t u, const E* e) {
    uintptr_t r = validRec(e, u);
    if (!r) { e->bad_b7880(); return; }
    uintptr_t p = e->f560d0_b7880(0);
    if (!p) { e->done_b7880(u); return; }
    uintptr_t path = e->rdp_b7880(u, kPath);
    if (cls1e(e, r) > 0 && e->o10203_b7880(path)) {
        Desc d; d.w[1] = p; d.w[2] = u; d.w[4] = (uintptr_t)cls1e(e, r);
        d.w[10] = (uintptr_t)e->o10240_b7880(u); d.w[12] = (uintptr_t)e->o11029_b7880(u);
        e->spawn_b7880(d.w);
    }
    int32_t n = e->rd32_b7880(r, 0x58); if (n < 1) n = 1;
    if ((int32_t)e->o10985_b7880(u) % n == 0) {
        int32_t a = e->o11018_b7880(u), b = e->o10818_b7880(u);
        uint64_t* s = e->seed_b7880(u);
        uint64_t v = (uint64_t)(uint32_t)*s * 0x6ac690c5ull + (uint32_t)(*s >> 32);
        *s = v;
        int32_t dx = a, dy = b;
        if ((v & 3) == 0) { dx = adj((a - b) * 3) >> 2; dy = adj((b + a) * 3) >> 2; }
        else if ((v & 3) == 2) { int32_t t = adj((b + a) * 3); a = -a; dx = t >> 2; dy = adj((b + a) * 3) >> 2; }
        int32_t ny = e->y_b7880(u) + dy, nx = e->x_b7880(u) + dx;
        e->o10286_b7880(path, nx, ny);
        e->o10334_b7880(path, u, 0);
        e->o10637_b7880(u, dx); e->o10019_b7880(u, dy);
    }
    e->done_b7880(u);
}

// 1.13c D2Client+0xB82B0 (284 B): spawn type-2 effect jittered within radius param3 of the owner.
// eax = owner unit (seed at +0x20), esi = rand range for the 0xA1330 fallback. Returns the spawned unit.
extern "C" EMSCRIPTEN_KEEPALIVE
uintptr_t d2_client_b82b0(uintptr_t eax, int32_t param1, int32_t param2, int32_t param3, uint32_t esi, const E* e) {
    Desc d; d.w[2] = 2; d.w[4] = (uintptr_t)param2; d.w[3] = (uintptr_t)param1;
    d.w[12] = (uintptr_t)e->o10240_b7880(eax); d.w[14] = (uintptr_t)e->o11029_b7880(eax);
    uint32_t m = (uint32_t)param3 * 2 + 1;
    int32_t dx = (int32_t)rnd(e, eax, m) - param3;
    int32_t ref = m < 1 ? param3 : param1;   // decompile reuses param_3 := param_1 on this path
    int32_t dy = (int32_t)rnd(e, eax, m) - ref;
    if (dx == 0 && dy == 0) dx = (int32_t)rnd(e, eax, esi) + 1;
    d.w[5] = (uintptr_t)dx; d.w[6] = (uintptr_t)dy;
    uintptr_t p = e->spawn_b7880(d.w);
    if (p) { e->o10637_b7880(p, dx); e->o10019_b7880(p, dy); }
    return p;
}

// 1.13c D2Client+0xB83D0 (293 B): frame-gated rectangle spawn (type 0x21). esi = rand range.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b83d0(uintptr_t u, uint32_t esi, const E* e) {
    uintptr_t r = validRec(e, u);
    if (!r || cls1e(e, r) < 0) { e->bad_b7880(); return; }
    int32_t d = e->rd32_b7880(r, 0x60) > 0 ? e->rd32_b7880(r, 0x60) : 1;
    if ((int32_t)e->o10985_b7880(u) % d != 0) { e->done_b7880(u); return; }
    int32_t a = (int32_t)rnd(e, u, esi) - e->rd32_b7880(r, 0x58);
    int32_t b = (int32_t)rnd(e, u, esi) - e->rd32_b7880(r, 0x58);
    int32_t cx = (int32_t)rnd(e, u, esi) - e->rd32_b7880(r, 0x5C) + e->x_b7880(u);
    int32_t cy = (int32_t)rnd(e, u, esi) - e->rd32_b7880(r, 0x5C) + e->y_b7880(u);
    Desc dd; dd.w[0] = (uintptr_t)a; dd.w[1] = (uintptr_t)b;
    dd.w[2] = 0x21; dd.w[3] = e->f560d0_b7880(0);
    dd.w[4] = (uintptr_t)cls1e(e, r);
    dd.w[7] = (uintptr_t)cx; dd.w[8] = (uintptr_t)cy;
    dd.w[9] = (uintptr_t)(cx + a); dd.w[10] = (uintptr_t)(cy + b);
    dd.w[12] = (uintptr_t)e->o10240_b7880(u); dd.w[14] = (uintptr_t)e->o11029_b7880(u);
    e->spawn_b7880(dd.w);
    e->done_b7880(u);
}

// 1.13c D2Client+0xB8550 (443 B): like 0xB8130 but 2-way sign flip drift, plus a post-spawn 0xC7E30.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b8550(uintptr_t u, const E* e) {
    uintptr_t r = validRec(e, u);
    if (!r) { e->bad_b7880(); return; }
    uintptr_t p = e->f560d0_b7880(0);
    if (!p) { e->done_b7880(u); return; }
    uintptr_t path = e->rdp_b7880(u, kPath);
    if (cls1e(e, r) >= 0 && e->o10203_b7880(path)) {
        Desc d; d.w[1] = p; d.w[2] = u; d.w[4] = (uintptr_t)cls1e(e, r);
        d.w[10] = (uintptr_t)e->o10240_b7880(u); d.w[12] = (uintptr_t)e->o11029_b7880(u);
        if (e->spawn_b7880(d.w)) e->f77e30_b7880(e->f77ec0_b7880(), 0, 0);
    }
    int32_t n = e->rd32_b7880(r, 0x58); if (n < 1) n = 1;
    if ((int32_t)e->o10985_b7880(u) % n == 0) {
        int32_t a = e->o11018_b7880(u), b = e->o10818_b7880(u);
        uint64_t* s = e->seed_b7880(u);
        uint64_t v = (uint64_t)(uint32_t)*s * 0x6ac690c5ull + (uint32_t)(*s >> 32);
        *s = v;
        int32_t c, f;
        if ((v & 1) == 0) { c = -a; f = b; } else { c = a; f = -b; }
        int32_t dx = adj((a + f) * 3) >> 2, dy = adj((b + c) * 3) >> 2;
        if (dx || dy) {
            int32_t ny = e->y_b7880(u) + dy, nx = e->x_b7880(u) + dx;
            e->o10286_b7880(path, nx, ny);
            e->o10334_b7880(path, u, 0);
        }
        e->o10637_b7880(u, dx); e->o10019_b7880(u, dy);
    }
    e->done_b7880(u);
}

// 1.13c D2Client+0xB86F0 (323 B): first-tick (11018==0) spawn of a type-2 effect jittered by rand,
// then advance. esi = rand range. GUESS: both rec+0x58 offsets as decompiled.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b86f0(uintptr_t u, uint32_t esi, const E* e) {
    uintptr_t r = validRec(e, u);
    if (!r || cls1e(e, r) < 0) { e->bad_b7880(); return; }
    uintptr_t p = e->f560d0_b7880(0);
    if (p) {
        int32_t v = e->o11018_b7880(u);
        if (v == 0) {
            Desc d; d.w[0] = 2; d.w[1] = p; d.w[2] = p; d.w[4] = (uintptr_t)cls1e(e, r);
            d.w[10] = (uintptr_t)e->o10240_b7880(u); d.w[12] = (uintptr_t)e->o11029_b7880(u);
            int32_t a = (int32_t)rnd(e, u, esi) - e->rd32_b7880(r, 0x58);
            int32_t b = (int32_t)rnd(e, u, esi) - e->rd32_b7880(r, 0x58);
            if (a == 0 && b == 0) a = (int32_t)rnd(e, u, esi) + 1;
            d.w[7] = (uintptr_t)a; d.w[8] = (uintptr_t)b;
            uintptr_t q = e->spawn_b7880(d.w);
            if (q) { e->o10637_b7880(q, a); e->o10019_b7880(q, b); }
            v = e->f60050_b7880();
        }
        e->o10637_b7880(u, v - 1);
    }
    e->done_b7880(u);
}

// 1.13c D2Client+0xB8840 (176 B): every rec+0x58 frames advance the unit's 64-step phase by rec+0x5c
// (mod 64, C-style signed remainder as MSVC emits) and spawn.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b8840(uintptr_t u, const E* e) {
    uintptr_t r = validRec(e, u);
    if (!r || cls1e(e, r) < 0) { e->bad_b7880(); return; }
    e->f560d0_b7880(0);
    int32_t n = e->rd32_b7880(r, 0x58) > 0 ? e->rd32_b7880(r, 0x58) : 1;
    if ((int32_t)e->o10985_b7880(u) % n < 1) {
        Desc d;
        d.w[10] = (uintptr_t)e->o10240_b7880(u); d.w[12] = (uintptr_t)e->o11029_b7880(u);
        uint32_t v = (uint32_t)e->o11018_b7880(u) & 0x8000003fu;
        if ((int32_t)v < 0) { v = (v - 1 | 0xffffffc0u) + 1; if ((int32_t)v < 0) v = (uint32_t)-(int32_t)v; }
        v = ((uint32_t)e->rd32_b7880(r, 0x5C) + v) & 0x8000003fu;
        if ((int32_t)v < 0) v = (v - 1 | 0xffffffc0u) + 1;
        e->o10637_b7880(u, (int32_t)v);
        e->spawn_b7880(d.w);
    }
    e->done_b7880(u);
}

// 1.13c D2Client+0xB8960 (~230 B): once frame >= rec byte +0x135 and the unit's path is live, spawn
// a type-1 follower at the unit and copy path state (10042/11080/11168) onto it.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b8960(uintptr_t u, const E* e) {
    uintptr_t r = validRec(e, u);
    uintptr_t path = u ? e->rdp_b7880(u, kPath) : 0;
    if (r && cls1e(e, r) >= 0 && (int32_t)e->o10985_b7880(u) >= (int32_t)e->rd8_b7880(r, 0x135)
        && e->o10203_b7880(path)) {
        int32_t x = e->x_b7880(u), y = e->y_b7880(u);
        Desc d; d.w[0] = 1; d.w[1] = e->f560d0_b7880(0);
        d.w[4] = (uintptr_t)cls1e(e, r); d.w[5] = (uintptr_t)x; d.w[6] = (uintptr_t)y;
        d.w[10] = (uintptr_t)e->o10240_b7880(u); d.w[12] = (uintptr_t)e->o11029_b7880(u);
        d.w[13] = (uintptr_t)e->rd32_b7880(r, 0x58);
        uintptr_t p = e->spawn_b7880(d.w);
        if (p) {
            uintptr_t pp = e->rdp_b7880(p, kPath);
            e->o10706_b7880(pp, e->o10042_b7880(path));
            e->o10236_b7880(pp, e->o11080_b7880(u));
            e->o10575_b7880(pp, e->o11168_b7880(u));
            e->f77e30_b7880(e->f77ec0_b7880(), 0, 0);
        }
    }
    e->done_b7880(u);
}
