// src/core/d2client/d2client_batch_b6290.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (0xB6290..0xB77A0),
// reimplemented from the cached decompiles. Alpha batch lane: implementation + marker only, no spec, no
// vector. Base 0x6FAB0000. These are the per-unit-type event emitters: each zeroes a 23-dword event
// record, fills it and hands it to FUN_6fb630f0 (0xB30F0, the event spawner). Unrecovered callees and
// all retail-memory reads are host seams on B6290Env (unique "b6290" names); event slots are widened to
// 64 bits so pointers fit, the slot index is the retail dword index.
// Guesses (decompiler lost stack-slot identity): event base for the 0x..9C-style calls is taken as the
// first zeroed local; slots a decompile discards (ordinal results with no named local) are assigned to
// the idx10 (Ordinal_10240) / idx12 (Ordinal_11029) pair seen in the unambiguous functions.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct B6290Env {
    // raw retail memory (32-bit game layout) and the sgptDataTables record array (+0xB6C count, +0xB64 base)
    uint32_t (*rd32_b6290)(uintptr_t addr);
    void     (*wr32_b6290)(uintptr_t addr, uint32_t v);
    int32_t  (*tbl_count_b6290)(void);
    uintptr_t(*tbl_base_b6290)(void);
    // 0xA60D0 FUN_6fb560d0: game/client context (arg is a stray register)
    uintptr_t(*ctx_b6290)(uintptr_t ecx);
    // 0xB30F0 event spawner; returns the created unit/object or 0
    uintptr_t(*spawn_b6290)(uint64_t* ev);
    void     (*fail_b6290)(void);                       // 0xB1720 common failure exit
    void     (*done_b6290)(uintptr_t unit);             // 0xB17C0 common success exit
    void     (*kill_b6290)(uintptr_t unit, int32_t f);  // 0xB1420
    void     (*aux_6b300_b6290)(uintptr_t p, int32_t a, int32_t b, int32_t c, int32_t d); // 0x6B300
    uintptr_t(*find_a1a50_b6290)(uintptr_t a, int32_t b, uintptr_t c, int32_t x, int32_t y); // 0xA1A50
    int32_t  (*q_a51f0_b6290)(void);                    // 0xA51F0
    int32_t  (*q_a5b90_b6290)(void);                    // 0xA5B90
    int32_t  (*q_a1340_b6290)(uint64_t* out);           // 0xA1340
    void     (*s_b2890_b6290)(uintptr_t unit);          // 0xB2890
    void     (*s_b2eb0_b6290)(uintptr_t unit);          // 0xB2EB0
    void     (*s_88a70_b6290)(uintptr_t p, int32_t a, int32_t b, int32_t c); // 0x88A70
    int32_t  (*unit_x_b6290)(uintptr_t u);              // 0x1630
    int32_t  (*unit_y_b6290)(uintptr_t u);              // 0x1660
    uintptr_t(*get15b0_b6290)(void);                    // 0x15B0
    uint32_t (*rnd1330_b6290)(uintptr_t seed);          // 0x1330
    uint32_t (*rnd1380_b6290)(uintptr_t seed);          // 0x1380
    // Ordinals (D2Common/D2Game), by number
    int32_t (*o10867)(uintptr_t); int32_t (*o10750)(uintptr_t);
    int32_t (*o10240)(uintptr_t); int32_t (*o11029)(uintptr_t);
    int32_t (*o11018)(uintptr_t); int32_t (*o10818)(uintptr_t); int32_t (*o10985)(uintptr_t);
    int32_t (*o10786)(uintptr_t, uintptr_t, int32_t, int32_t);
    void    (*o10637)(uintptr_t, int32_t); void (*o10024)(uintptr_t, uintptr_t);
    void    (*o10019)(uintptr_t, int32_t);
    int32_t (*o10255)(uintptr_t); int32_t (*o10239)(uintptr_t);
    void    (*o10647)(uint32_t, int32_t); void (*o10334)(uint32_t, uintptr_t, int32_t);
    int32_t (*o10614)(uintptr_t); int32_t (*o10203)(uint32_t); int32_t (*o10331)(uintptr_t);
    int16_t (*o10482)(uintptr_t, int32_t, int32_t, int32_t);
    // module data tables, 8 x int32 each
    const int32_t* tab_83bb8; const int32_t* tab_83bd8;
    const int32_t* tab_83d4c; const int32_t* tab_83d2c;
};

namespace {
struct Ev { uint64_t w[23]; Ev() { for (auto& x : w) x = 0; } };
inline uint32_t U(const B6290Env* e, uintptr_t u, int i) { return e->rd32_b6290(u + 4 * i); }
inline int16_t  S(const B6290Env* e, uintptr_t r, int off) { return (int16_t)(e->rd32_b6290(r + off) & 0xffff); }
inline uint32_t R(const B6290Env* e, uintptr_t r, int off) { return e->rd32_b6290(r + off); }
// sgptDataTables record for txt index idx, 0 if out of range
inline uintptr_t rec(const B6290Env* e, int32_t idx) {
    if (idx < 0 || idx >= e->tbl_count_b6290()) return 0;
    return e->tbl_base_b6290() + (uintptr_t)idx * 0x1a4;
}
inline bool trio(uint32_t t) { return t == 2 || (t > 3 && t < 6); }
}

// 1.13c D2Client+0xB6290 (eax = unit, ecx = unused)
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_b6290(uintptr_t unit, uint32_t ecx, int32_t txt, uint32_t p3, uint32_t p4, int32_t p5,
                         int32_t p6, const B6290Env* e) {
    (void)ecx;
    uintptr_t r = rec(e, txt);
    if (!r) return 0;
    Ev ev; uint32_t t = U(e, unit, 0); uintptr_t u2 = U(e, unit, 0xb);
    ev.w[0] = 0x1f; ev.w[1] = p3; ev.w[4] = (uint32_t)txt;
    ev.w[5] = trio(t) ? R(e, u2, 0xc) : (u2 == 0 ? 0 : (uint32_t)e->o10867(u2));
    ev.w[6] = trio(t) ? R(e, u2, 0x10) : (u2 == 0 ? 0 : (uint32_t)e->o10750(u2));
    ev.w[12] = p5; ev.w[13] = p5 - 1; ev.w[10] = R(e, r, 0x38) << 7; ev.w[11] = p4;
    for (int32_t i = 0; i < 0x20; i += p6 * 4) {
        ev.w[8] = (uint32_t)e->tab_83bb8[i / 4]; ev.w[7] = (uint32_t)e->tab_83bd8[i / 4];
        e->spawn_b6290(ev.w);
    }
    return 1;
}

// 1.13c D2Client+0xB67D0 (ecx = unit, edx = obj; ebx is read uninitialised on one path -> explicit)
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_b67d0(uintptr_t unit, uintptr_t obj, uintptr_t ebx, const B6290Env* e) {
    if (!unit) return 1;
    uintptr_t r = rec(e, (int32_t)U(e, unit, 1));
    if (!r) return 1;
    uintptr_t g = e->ctx_b6290(0);
    if (!g || !obj) return 1;
    if (S(e, r, 0x36) > 0) e->aux_6b300_b6290(obj, 2, 0, 0, 0);
    if (e->o11018(unit) <= 1) return 1;
    int32_t x = e->unit_x_b6290(unit); uint32_t y = (uint32_t)e->unit_y_b6290(unit);
    int32_t v = (int32_t)R(e, r, 0x6c);
    if (v < 1) {
        int32_t a = e->o10240(unit), b = e->o11029(unit);
        uintptr_t p = e->get15b0_b6290();
        if (!p) return 1;
        v = e->o10786(g, R(e, p, 100), a, b);
        obj = ebx;
        if (v < 1) v = 1;
    }
    uintptr_t t = e->find_a1a50_b6290(U(e, obj, 3), v, g, x, (int32_t)y);
    if (t && t != obj) {
        Ev ev; ev.w[0] = 0x21; ev.w[1] = g; ev.w[3] = t; ev.w[4] = U(e, unit, 1);
        ev.w[5] = (uint32_t)x; ev.w[6] = y;
        ev.w[7] = (uint32_t)e->unit_x_b6290(t); ev.w[8] = (uint32_t)e->unit_y_b6290(t);
        e->o10240(unit); ev.w[10] = (uint32_t)e->o11029(unit);
        uintptr_t n = e->spawn_b6290(ev.w);
        if (n) { e->o10637(n, x - 1); e->o10024(n, unit); }
    }
    return 1;
}

// 1.13c D2Client+0xB6990 (ecx = unit, edx = p2)
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_b6990(uintptr_t unit, uintptr_t p2, const B6290Env* e) {
    if (!unit) return 1;
    uintptr_t r = rec(e, (int32_t)U(e, unit, 1));
    if (!r || !p2) return 1;
    uintptr_t g = e->ctx_b6290(0);
    if (R(e, r, 0x6c) == 0 || !g || (!e->q_a51f0_b6290() && !e->q_a5b90_b6290())) {
        uint32_t k = R(e, p2, 0);
        if (k == 0) { if (R(e, r, 0x70) != 0) return 0; }
        else {
            if (k != 1) return 0;
            if (R(e, r, 0x70) == 2) { if (!e->o10255(p2)) return 0; }
            else if (!e->o10239(p2)) return 0;
        }
        if (S(e, r, 0x2c) >= 0) {
            Ev ev; ev.w[1] = g; e->o10240(unit); ev.w[10] = (uint32_t)e->o11029(unit); ev.w[2] = 0xcc;
            e->spawn_b6290(ev.w);
        }
    } else if (S(e, r, 0x36) > 0) e->aux_6b300_b6290(p2, 2, 0, 0, 0);
    return 1;
}

// 1.13c D2Client+0xB6AC0 (ecx = unit)
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_b6ac0(uintptr_t unit, const B6290Env* e) {
    uintptr_t g = e->ctx_b6290(unit);
    if (!g || !unit) return 1;
    uintptr_t r = rec(e, (int32_t)U(e, unit, 1));
    if (!r || S(e, r, 0x2c) < 0) return 1;
    Ev ev; ev.w[0] = 2; ev.w[1] = g; ev.w[2] = unit;
    e->o10240(unit); ev.w[10] = (uint32_t)e->o11029(unit);
    int32_t v = (int32_t)R(e, r, 0x6c); if (v < 1) v = 1;
    for (int32_t i = 0; i < 0x20; i += v * 4) {
        ev.w[5] = (uint32_t)e->tab_83d4c[i / 4]; ev.w[6] = (uint32_t)e->tab_83d2c[i / 4];
        uintptr_t p = e->spawn_b6290(ev.w);
        if (p) { e->o10637(p, e->tab_83d4c[i / 4]); e->o10019(p, e->tab_83d2c[i / 4]); }
    }
    return 1;
}

// 1.13c D2Client+0xB6BC0 (eax = unit, ebx = flag, edx = p2, stack = s1, s2). Per-iteration slots were
// lost by the decompiler; only the type/context/loop shape is recovered.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b6bc0(uintptr_t unit, uint32_t ebx, int32_t p2, int32_t s1, int32_t s2, const B6290Env* e) {
    if (!ebx) return;
    Ev ev; ev.w[0] = p2 > 0 ? 0x1f : 0x17; ev.w[1] = e->ctx_b6290(0);
    uint32_t t = U(e, unit, 0); uintptr_t u2 = U(e, unit, 0xb);
    if (t != 2 && (t < 4 || t > 5) && u2) { ev.w[5] = (uint32_t)e->o10867(u2); ev.w[6] = (uint32_t)e->o10750(u2); }
    e->o10240(unit); ev.w[10] = (uint32_t)e->o11029(unit);
    if (s1 < 1) s1 = 1;
    for (int32_t i = 0; i < 0x40; i += s1 * 4) e->spawn_b6290(ev.w);
    if (s2 > 0) for (int32_t i = 0; i < 0x3c; i += s2 * 4) e->spawn_b6290(ev.w);
}

// 1.13c D2Client+0xB6F60 (ecx = unit)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b6f60(uintptr_t unit, const B6290Env* e) {
    uintptr_t r = unit ? rec(e, (int32_t)U(e, unit, 1)) : 0;
    if (!r || S(e, r, 0x1e) < 0) { e->fail_b6290(); return; }
    uintptr_t g = e->ctx_b6290(0);
    if (!g) { e->fail_b6290(); return; }
    e->s_b2890_b6290(unit);
    Ev scratch;
    if (!e->q_a1340_b6290(scratch.w + 1)) return;
    int32_t a = e->o10818(unit), b = e->o11018(unit);
    if (a < b) e->o10019(unit, a + 1);
    Ev ev; ev.w[0] = 0x20; ev.w[1] = g;
    e->o10240(unit); ev.w[10] = (uint32_t)e->o11029(unit);
    ev.w[2] = (int64_t)S(e, r, 0x1e);
    uintptr_t p = e->spawn_b6290(ev.w);
    if (p) { e->o10647(R(e, p, 0x2c), 0xe); e->o10334(R(e, p, 0x2c), p, 0); }
    e->o10019(unit, 0);
    e->done_b6290(unit);
}

// 1.13c D2Client+0xB70A0 (ecx = unit)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b70a0(uintptr_t unit, const B6290Env* e) {
    uintptr_t r = unit ? rec(e, (int32_t)U(e, unit, 1)) : 0;
    if (!r) { e->fail_b6290(); return; }
    int32_t n = e->o10985(unit);
    uint32_t cnt = 0;
    while (cnt < 3 && S(e, r, 0x1e + 2 * cnt) >= 0 && (int32_t)R(e, r, 0x5c + 4 * cnt) >= 1) cnt++;
    if (cnt == 0) { e->kill_b6290(unit, 0); return; }
    if ((int32_t)R(e, r, 0x58) <= n) {
        uint32_t t = U(e, unit, 0); uintptr_t u2 = U(e, unit, 0xb);
        if (t != 2 && (t < 4 || t > 5) && u2) e->o10867(u2);
        e->o10614(unit);
        uint32_t lo = U(e, unit, 8), hi = U(e, unit, 9);
        uint64_t s = (uint64_t)lo * 0x6ac690c5u + hi;
        e->wr32_b6290(unit + 0x20, (uint32_t)s); e->wr32_b6290(unit + 0x24, (uint32_t)(s >> 32));
        uint32_t k = (cnt & (cnt - 1)) == 0 ? ((cnt - 1) & (uint32_t)s) : (uint32_t)s % cnt;
        int32_t div = (int32_t)R(e, r, 0x5c + 4 * k);
        if (n % div == 0) {
            Ev ev; ev.w[1] = e->ctx_b6290(0); ev.w[2] = unit;
            ev.w[4] = (int64_t)S(e, r, 0x1e + 2 * k);
            ev.w[10] = (uint32_t)e->o10240(unit); ev.w[12] = (uint32_t)e->o11029(unit);
            uintptr_t p = e->spawn_b6290(ev.w);
            if (p) e->kill_b6290(p, 1);
        }
    }
    e->done_b6290(unit);
}

// 1.13c D2Client+0xB7220 (ecx = unit)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b7220(uintptr_t unit, const B6290Env* e) {
    uintptr_t r = unit ? rec(e, (int32_t)U(e, unit, 1)) : 0;
    if (!r || S(e, r, 0x1e) < 0) { e->fail_b6290(); return; }
    if (e->o10203(U(e, unit, 0xb))) {
        Ev ev; ev.w[1] = e->ctx_b6290(0); ev.w[4] = (int64_t)S(e, r, 0x1e);
        ev.w[10] = (uint32_t)e->o10240(unit); ev.w[12] = (uint32_t)e->o11029(unit);
        e->spawn_b6290(ev.w);
    }
    e->s_b2eb0_b6290(unit);
}

// 1.13c D2Client+0xB72D0 (ecx = unit)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b72d0(uintptr_t unit, const B6290Env* e) {
    uintptr_t r = unit ? rec(e, (int32_t)U(e, unit, 1)) : 0;
    if (!r || S(e, r, 0x1e) < 0 || !e->ctx_b6290(0)) { e->fail_b6290(); return; }
    if (e->o10203(U(e, unit, 0xb))) {
        int32_t n = e->o11018(unit), m = e->o10818(unit);
        Ev ev; ev.w[0] = unit; e->o10240(unit); ev.w[12] = (uint32_t)e->o11029(unit);
        ev.w[4] = (int64_t)S(e, r, 0x1e); ev.w[7] = (uint32_t)n; ev.w[8] = (uint32_t)m;
        e->spawn_b6290(ev.w);
        ev.w[7] = (uint32_t)-n; ev.w[8] = (uint32_t)-m;
        e->spawn_b6290(ev.w);
    }
    e->done_b6290(unit);
}

// 1.13c D2Client+0xB73E0 (ecx = unit)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b73e0(uintptr_t unit, const B6290Env* e) {
    uintptr_t r = unit ? rec(e, (int32_t)U(e, unit, 1)) : 0;
    if (!r) { e->fail_b6290(); return; }
    int32_t n = e->o10985(unit);
    if (S(e, r, 0x1e) >= 0 && n == (int32_t)R(e, r, 0x58)) {
        int32_t x = e->unit_x_b6290(unit), y = e->unit_y_b6290(unit);
        Ev ev; ev.w[4] = (int64_t)S(e, r, 0x1e); ev.w[0] = 0x2001; ev.w[1] = e->ctx_b6290(ev.w[4]);
        ev.w[10] = (uint32_t)e->o10240(unit); ev.w[12] = (uint32_t)e->o11029(unit);
        ev.w[5] = (uint32_t)x; ev.w[6] = (uint32_t)y;
        e->spawn_b6290(ev.w);
        int32_t k = (int32_t)R(e, r, 100);
        for (int32_t i = 0; i < (int32_t)R(e, r, 0x60); i++) {
            ev.w[5] = e->rnd1330_b6290(unit + 0x20) + (uint32_t)(x - k);
            ev.w[6] = e->rnd1330_b6290(unit + 0x20) + (uint32_t)(y - k);
            e->spawn_b6290(ev.w);
        }
    }
    if (S(e, r, 0x20) >= 0 && n == (int32_t)R(e, r, 0x5c)) {
        uintptr_t g = e->ctx_b6290(0);
        if (g) {
            uint32_t f = R(e, g, 0xc8);
            Ev ev; e->wr32_b6290(g + 0xc8, f | 0x40000);
            ev.w[4] = (int64_t)S(e, r, 0x20); ev.w[0] = 0x2000; ev.w[1] = g; ev.w[2] = unit;
            ev.w[10] = (uint32_t)e->o10240(unit); ev.w[12] = (uint32_t)e->o11029(unit);
            uintptr_t p = e->spawn_b6290(ev.w);
            if (p && S(e, r, 0x34) > 0) e->s_88a70_b6290(p, 0, 0, 0);
        }
    }
    e->done_b6290(unit);
}

// 1.13c D2Client+0xB75C0 (ecx = unit)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b75c0(uintptr_t unit, const B6290Env* e) {
    uintptr_t r = unit ? rec(e, (int32_t)U(e, unit, 1)) : 0;
    uintptr_t g = (r && S(e, r, 0x1e) >= 0) ? e->ctx_b6290(0) : 0;
    if (!g || !e->o10331(unit)) { e->fail_b6290(); return; }
    uintptr_t q = (uintptr_t)e->o10240(unit);   // used as a record pointer below (+0x20 short)
    uintptr_t c = e->get15b0_b6290();
    if (!c) { e->fail_b6290(); return; }
    e->o11029(unit);
    uintptr_t g0 = R(e, c, 0x138);
    uintptr_t g1 = e->ctx_b6290(0);
    int32_t six = e->o10786(g1, 0, 0, 0);
    uint32_t h = R(e, c, 0x13c);
    int32_t n = e->o10786(e->ctx_b6290(h), h, (int32_t)q, (int32_t)unit);
    if (n < 1) n = 1;
    if (e->o10985(unit) % n == 0) {
        int32_t x0 = e->unit_x_b6290(unit); e->o10614(unit);
        uint32_t u9 = e->rnd1380_b6290(unit + 0x20);
        int32_t x = e->unit_x_b6290(unit);
        uint32_t u10 = e->rnd1380_b6290(unit + 0x20);
        int32_t y = e->unit_y_b6290(unit); (void)x0;
        if (e->o10482(g1, (int32_t)(u9 - (six - 1)) + x, (int32_t)(u10 - (six - 1)) + y, 0x45) == 0) {
            Ev ev; ev.w[0] = 0x2001; ev.w[1] = g0;
            ev.w[10] = (uint32_t)e->o10240(unit); ev.w[12] = (uint32_t)e->o11029(unit);
            uintptr_t p = e->spawn_b6290(ev.w);
            if (p) e->wr32_b6290(p + 0xc4, R(e, p, 0xc4) | 0x10000);
            if (S(e, q, 0x20) >= 0) e->spawn_b6290(ev.w);
        }
    }
    e->done_b6290(unit);
}

// 1.13c D2Client+0xB77A0 (ecx = unit)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b77a0(uintptr_t unit, const B6290Env* e) {
    uintptr_t r = unit ? rec(e, (int32_t)U(e, unit, 1)) : 0;
    if (!r || S(e, r, 0x1e) == 0) { e->fail_b6290(); return; }
    if (e->o10203(U(e, unit, 0xb))) {
        Ev ev; ev.w[1] = e->ctx_b6290(0);
        ev.w[5] = (uint32_t)e->unit_x_b6290(unit); ev.w[6] = (uint32_t)e->unit_y_b6290(unit);
        ev.w[10] = (uint32_t)e->o10240(unit); ev.w[12] = (uint32_t)e->o11029(unit);
        e->spawn_b6290(ev.w);
    }
    e->done_b6290(unit);
}
