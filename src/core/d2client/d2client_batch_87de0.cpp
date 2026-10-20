// src/core/d2client/d2client_batch_87de0.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (0x87DE0..0x888B0, the
// sound-entry list at DAT_6fbcbf30) reimplemented from the cached decompiles. Alpha batch lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector. Env-seam style of d2client_batch_5d110.cpp.
// List node = {entry*, next*}; entry fields at retail byte offsets (packed, unaligned). Record table stride 0x8E.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

struct D2cSnd87 {
    void**   list;     // +0x10BF30 DAT_6fbcbf30 head
    uint8_t* recs;     // +0x10C24C
    int32_t  count;    // +0x10C250
    int32_t  now;      // +0x10C268
    int32_t  lo, hi;   // +0x10C25C / +0x10C260
    uint32_t player;   // +0x10BBFC
};
struct D2cSnd87Env {
    uint32_t (*ord10032)(void);
    void     (*ord10006)(void);
    void     (*ord10062)(void);   // per-node call in 88150 (args implicit in retail)
    void     (*ord10063)(void);
    void     (*fn87ed0)(int32_t now, uint32_t key, int32_t mode);
    // Ordinal_10628: unit/pos query; retail's decompile of 0x881B0 is stack-garbled, modelled as point out.
    void     (*ord10628)(uint32_t player, int32_t* x, int32_t* y);
    int64_t  limit;               // _DAT_6fb87570 compared against dx^2+dy^2
    void     (*fatal)(uint32_t code); // Ordinal_10265+10024+_exit(-1); no return
};

template <class T> static T rd(const uint8_t* p, size_t o) { T v; memcpy(&v, p + o, sizeof v); return v; }
template <class T> static void wr(uint8_t* p, size_t o, T v) { memcpy(p + o, &v, sizeof v); }
static uint8_t* rec(const D2cSnd87* s, int32_t i) {
    return (!s->recs || i < 0 || i >= s->count) ? nullptr : s->recs + (size_t)i * 0x8E;
}
static uint8_t* ent(void** n) { return (uint8_t*)n[0]; }
static uint8_t* find(const D2cSnd87* s, int32_t key) {
    if (!key) return nullptr;
    for (void** n = s->list; n; n = (void**)n[1]) if (rd<int32_t>(ent(n), 8) == key) return ent(n);
    return nullptr;
}

// 1.13c D2Client+0x87DE0 (key in EDX)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_87de0(D2cSnd87* s, const D2cSnd87Env* e, int32_t key) {
    uint8_t* p = find(s, key);
    if (!p) return 0;
    if (rd<uintptr_t>(p, 0x2c)) return e->ord10032();
    e->fatal(0x45a);
    return 0;
}

// 1.13c D2Client+0x87E90
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_87e90(D2cSnd87* s, const D2cSnd87Env*, int32_t key) {
    uint8_t* p = find(s, key);
    if (!p) return 0;
    uintptr_t a = rd<uintptr_t>(p, 0x28);
    return a && rd<int32_t>((const uint8_t*)a, 4) ? 1 : 0;
}

// 1.13c D2Client+0x87FE0 (idx in EAX, skip entry param_1)
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t* d2_client_87fe0(D2cSnd87* s, const D2cSnd87Env*, int32_t idx, uint8_t* skip) {
    uint8_t* r = rec(s, idx);
    for (void** n = s->list; n; n = (void**)n[1]) {
        uint8_t* p = ent(n);
        uint8_t* q = rec(s, rd<int32_t>(p, 4));
        int32_t grp = s->recs ? rd<int32_t>(q, 0x60) : 0;   // retail reads through null on bad idx
        if (grp == rd<int32_t>(r, 0x60) && p != skip && rd<int32_t>(p, 0x3d) == 0) {
            int16_t w = rd<int16_t>(r, 0x45);
            if (w < 0 || (uint32_t)(s->now - rd<int32_t>(p, 0x34)) <= (uint32_t)(int32_t)w) return p;
        }
    }
    return nullptr;
}

// 1.13c D2Client+0x880E0 (entry in EDI)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_880e0(D2cSnd87*, const D2cSnd87Env* e, uint8_t* p) {
    if (rd<int32_t>(p, 0x30) == 1) {
        uint8_t* b = (uint8_t*)rd<uintptr_t>(p, 0x2c);
        if (rd<int32_t>(b, 0)) {
            uintptr_t c = rd<uintptr_t>(b, 8);
            if (c) wr<uint32_t>((uint8_t*)c, 0x41, e->ord10032());
            e->ord10006();
        }
        uintptr_t c = rd<uintptr_t>(b, 8);
        if (c) {
            wr<int32_t>((uint8_t*)c, 0x30, 2);
            wr<uintptr_t>((uint8_t*)c, 0x2c, 0);
            wr<uintptr_t>(b, 8, 0);
        }
    }
    if (rd<uintptr_t>(p, 0x2c)) e->fatal(0x93);
}

// 1.13c D2Client+0x88150 (entry in EDI); chain nodes are {item*, next*}, item[0] must be nonzero
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_88150(D2cSnd87*, const D2cSnd87Env* e, uint8_t* p) {
    for (void** n = (void**)rd<uintptr_t>(p, 0x28); n; n = (void**)n[1]) {
        if (n[0] == nullptr) { e->fatal(0xc2c); return; }
        e->ord10062();
    }
    e->ord10063();
}

// 1.13c D2Client+0x881B0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_881b0(D2cSnd87* s, const D2cSnd87Env* e) {
    int32_t ax, ay, bx, by;
    e->ord10628(s->player, &ax, &ay);
    e->ord10628(s->player, &bx, &by);
    int64_t dx = (int64_t)(ax - bx) * 2, dy = (int64_t)(by - ay);
    return (dx * dx + dy * dy) < e->limit ? 1 : 0;
}

// 1.13c D2Client+0x88230 (this ignored, eax = base duration in EAX on entry)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_88230(D2cSnd87* s, const D2cSnd87Env* e, int32_t key, int32_t mode,
                                                     int32_t delay, uint32_t eax) {
    if (!key) return;
    uint8_t* p = find(s, key);
    if (!p) return;
    if (d2_client_87e90(s, e, key)) return;
    uint8_t* r = rec(s, rd<int32_t>(p, 4));
    uint32_t dur = eax;
    if (mode == 0xff || mode == 0) {
        uint8_t b = rd<uint8_t>(r, mode == 0 ? 0x40 : 0x3f);
        if (dur < b) dur = b;
    }
    bool tail = false;   // reach the "if (dur)" block
    if (rd<int32_t>(p, 0x45) == 0) {
        if (mode == 0) wr<int32_t>(p, 0x3d, 1);
        tail = true;
    } else {
        if (mode == rd<int32_t>(p, 0x4d) && rd<uint32_t>(p, 0x55) <= (uint32_t)(delay + s->now) + dur) return;
        if (rd<int32_t>(p, 0x4d) != 0) { if (mode == 0) wr<int32_t>(p, 0x3d, 1); tail = true; }
        else { if (mode != 0) return; wr<int32_t>(p, 0x3d, 1); tail = true; }
    }
    (void)tail;
    if (dur) {
        int32_t t = delay + s->now;
        wr<int32_t>(p, 0x51, t);
        wr<uint32_t>(p, 0x49, rd<uint32_t>(p, 0x1c));
        wr<int32_t>(p, 0x4d, mode);
        wr<uint32_t>(p, 0x55, (uint32_t)t + dur);
        wr<int32_t>(p, 0x45, 1);
        return;
    }
    if (delay) { e->fatal(0x25c); return; }
    e->fn87ed0(s->now, (uint32_t)key, mode);
}

static void snd_kick(D2cSnd87* s, const D2cSnd87Env* e, uint8_t* p) {   // 1.13c D2Client+0x88550 body
    uint8_t* r = rec(s, rd<int32_t>(p, 4));
    wr<int32_t>(p, 0x3d, 1);
    int32_t k = rd<int32_t>(p, 8);
    if (rd<uint8_t>(r, 0x40) != 0 && rd<int32_t>(p, 0x30) == 1) d2_client_88230(s, e, k, 0, 0, 0);
}

// 1.13c D2Client+0x88550 (entry in ECX)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_88550(D2cSnd87* s, const D2cSnd87Env* e, uint8_t* p) { snd_kick(s, e, p); }

// shared walk: kick every entry whose record index satisfies pred
template <class F> static void walk(D2cSnd87* s, const D2cSnd87Env* e, F pred) {
    for (void** n = s->list; n; n = (void**)n[1]) if (pred(ent(n))) snd_kick(s, e, ent(n));
}

// 1.13c D2Client+0x886F0 (idx 0xb76..0x1230)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_886f0(D2cSnd87* s, const D2cSnd87Env* e) {
    walk(s, e, [](uint8_t* p) { int32_t i = rd<int32_t>(p, 4); return i > 0xb75 && i < 0x1231; });
}

// 1.13c D2Client+0x88770 (idx 0x48..0xc9, group != a and != b)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_88770(D2cSnd87* s, const D2cSnd87Env* e, int32_t a, int32_t b) {
    walk(s, e, [&](uint8_t* p) {
        int32_t i = rd<int32_t>(p, 4); uint8_t* r = rec(s, i);
        int32_t g = s->recs ? rd<int32_t>(r, 0x60) : 0;
        return g != a && g != b && i > 0x47 && i < 0xca;
    });
}

// 1.13c D2Client+0x88810 (idx 0x34..0x47, group != a and != b)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_88810(D2cSnd87* s, const D2cSnd87Env* e, int32_t a, int32_t b) {
    walk(s, e, [&](uint8_t* p) {
        int32_t i = rd<int32_t>(p, 4); uint8_t* r = rec(s, i);
        int32_t g = s->recs ? rd<int32_t>(r, 0x60) : 0;
        return g != a && g != b && i > 0x33 && i < 0x48;
    });
}

// 1.13c D2Client+0x888B0 (idx lo..hi)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_888b0(D2cSnd87* s, const D2cSnd87Env* e) {
    walk(s, e, [&](uint8_t* p) { int32_t i = rd<int32_t>(p, 4); return s->lo <= i && i <= s->hi; });
}
