// src/core/d2client/d2client_batch_5d110.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (0x5D110..0x5EB90, the
// 0x8E-byte resource-record table and its 16-slot pick table) reimplemented from the cached decompiles.
// Alpha batch lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector. Base 0x6FAB0000.
// Unrecovered callees (ordinals, other RVAs) are host seams (the d2client_batch_595c0.cpp pattern).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

struct D2cRes5d {
    uint8_t*  recs;      // +0x10C24C DAT_6fbcc24c record array, stride 0x8E
    int32_t   count;     // +0x10C250
    int32_t   now;       // +0x10C268 tick stamp
    void**    list;      // +0x10C264 list node: [0]=record index, [1]=next node
    int32_t   used;      // +0x10C27C
    int32_t   live;      // +0x10C274
    int32_t   inited;    // +0x10C280
    uint32_t  player;    // +0x10BBFC
    struct Slot {        // +0x1175C0, 16 x 0x20; own layout, retail offsets noted
        uint32_t h;      // +0x00
        uint32_t mode;   // +0x04
        uint8_t* rec;    // +0x08
        uint8_t  pad[0x10];
    } slot[16];
};
static_assert(sizeof(D2cRes5d::Slot) >= 0x18, "slot");

struct D2cRes5dEnv {
    int      (*ord10041)(void);
    void     (*ord10061)(void);
    uint32_t (*ord10032)(void);
    void     (*ord10006)(void);
    void     (*ord10049)(void);
    uint32_t (*ord10065)(void);          // slot handle alloc, 0 = fail
    int      (*ord10056)(void);
    int      (*ord10046)(void);
    int      (*ord10059)(void);
    void     (*ord10066)(void);
    uint32_t (*ord10094)(void);
    uint32_t (*ord10095)(void);
    void     (*ord10062)(void);
    void     (*ord10018)(void);
    void     (*ord10097)(void);
    void     (*ord10043)(uint32_t a, uint32_t b);
    uint32_t (*ord10331)(uint32_t);
    uint32_t (*ord10826)(uint32_t);
    uint32_t (*ord10014)(uint32_t);
    void     (*fn5cf90)(void);
    void     (*fn5cd10)(void);
    int32_t* (*fn5cc10)(int32_t idx);    // 0x5CC10 pick lookup
    void     (*fatal)(uint32_t code);    // Ordinal 10265+10024+_exit(-1); no return
    const char* sufA;                    // DAT_6fb853e4 (6 bytes incl NUL)
    const char* sufB;                    // DAT_6fb853ec (8 bytes incl NUL)
    uint8_t  (*ord14byte)(uint32_t unit);// reads +0x21C of Ordinal_10014 result
};

static uint8_t* rec(const D2cRes5d* s, int32_t i) {
    return (!s->recs || i < 0 || i >= s->count) ? nullptr : s->recs + (size_t)i * 0x8E;
}
template <class T> static T rd(const uint8_t* p, size_t o) { T v; memcpy(&v, p + o, sizeof v); return v; }
template <class T> static void wr(uint8_t* p, size_t o, T v) { memcpy(p + o, &v, sizeof v); }

// 1.13c D2Client+0x5D110
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_5d110(D2cRes5d* s, const D2cRes5dEnv* e) {
    if (!e->ord10041()) return;
    e->ord10061();
    for (auto& sl : s->slot) {
        if (sl.h) {                       // piVar3[-2]
            if (sl.rec) wr<uint32_t>(sl.rec, 0x41, e->ord10032());
            e->ord10006();
        }
        if (sl.rec) {
            wr<uint32_t>(sl.rec, 0x30, 2);
            wr<uint32_t>(sl.rec, 0x2c, 0);
            sl.rec = nullptr;
        }
    }
    e->ord10061();
}

// 1.13c D2Client+0x5D180
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_5d180(D2cRes5d* s, const D2cRes5dEnv* e) {
    if (!s->inited) return;
    d2_client_5d110(s, e);
    for (auto& sl : s->slot) { e->ord10049(); sl.h = 0; }
    s->inited = 0;
}

// 1.13c D2Client+0x5D540 (mode arrives in EAX)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_5d540(D2cRes5d* s, const D2cRes5dEnv* e, int32_t mode) {
    if (!e->ord10041()) return 0;
    if (s->inited) e->fatal(0xCC);
    for (int i = 0; i < 16; i++) {
        auto& sl = s->slot[i];
        sl.mode = i < 4 ? 1 : mode == 1 ? 2 : (uint32_t)(((mode != 2) - 1) & 6);
        sl.h = e->ord10065();
        if (!sl.h) { d2_client_5d180(s, e); return 0; }
    }
    e->fn5cf90();
    s->inited = 1;
    return 1;
}

// 1.13c D2Client+0x5D600 (mode arrives in ESI)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_5d600(D2cRes5d* s, const D2cRes5dEnv* e, int32_t mode) {
    if (mode == 1 || mode == 2) {
        if ((mode == 1 ? e->ord10056() : e->ord10046()) == 0) return 0;
    }
    d2_client_5d180(s, e);
    if (!d2_client_5d540(s, e, mode)) return 0;
    if (e->ord10059() != mode) e->ord10066();
    return 1;
}

// 1.13c D2Client+0x5DA80 (record in EAX)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_5da80(D2cRes5d* s, const D2cRes5dEnv* e, uint8_t* r) {
    if (rd<int32_t>(r, 0x86) != 1) e->fatal(0x40);
    wr<uint32_t>(r, 0x6c, e->ord10094());
    wr<uint32_t>(r, 0x70, e->ord10095());
    wr<int32_t>(r, 0x86, 2);
    e->fn5cd10();
    s->live--;
}

// 1.13c D2Client+0x5DAE0 (index in ESI; record is null-checked-by-range, retail writes through null if bad)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_5dae0(D2cRes5d* s, const D2cRes5dEnv* e, int32_t idx) {
    uint8_t* r = rec(s, idx);
    if (r) wr<int32_t>(r, 0x7c, s->now);
    e->ord10062();
    e->ord10018();
}

// 1.13c D2Client+0x5DB30 (this=idx, out buffer param_1); out holds path < 0x50 chars
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_5db30(D2cRes5d* s, const D2cRes5dEnv* e, int32_t idx, char* out) {
    const char* name = (const char*)rec(s, idx);
    strcpy(out, (idx < 0xb76 || idx > 0x1230) ? "DATA\\GLOBAL" : "DATA\\LOCAL");
    if (idx < 0x1231 || idx > 0x125a) memcpy(out + strlen(out), e->sufA, 6);
    else memcpy(out + strlen(out), e->sufB, 8);
    if (!(strlen(out) + strlen(name) < 0x50)) e->fatal(0x3ab);
    strcat(out, name);
}

// 1.13c D2Client+0x5DD50 (index in EDI)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_5dd50(D2cRes5d* s, const D2cRes5dEnv* e, int32_t idx) {
    uint8_t* r = rec(s, idx);
    if (rd<int32_t>(r, 0x86) == 0) return;
    if (e->fn5cc10(idx)) e->fatal(0x259);
    s->used -= rd<int32_t>(r, 0x70);
    e->ord10062();
    wr<int32_t>(r, 0x7c, 0);
    if (rd<int32_t>(r, 0x82)) {
        e->ord10097();
        wr<int32_t>(r, 0x82, 0);
    } else {
        e->ord10043(0x272, 0);
    }
    wr<int32_t>(r, 0x6c, 0);
    wr<int32_t>(r, 0x86, 0);
}

// 1.13c D2Client+0x5DE10 (param_1 in ECX): zero +0x8A over the child run [+0x60, +0x60+byte(+0x3D))
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_5de10(D2cRes5d* s, int32_t i) {
    uint8_t* p = i ? rec(s, i) : nullptr;
    if (!p) return;
    int32_t first = rd<int32_t>(p, 0x60);
    for (int32_t k = first; k < (int32_t)p[0x3d] + rd<int32_t>(p, 0x60); k++)
        if (uint8_t* c = rec(s, k)) wr<int32_t>(c, 0x8a, 0);
}

// 1.13c D2Client+0x5DE90 (param_1 in ECX, delta in EBX): add delta to +0x8A over the child run
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_5de90(D2cRes5d* s, const D2cRes5dEnv* e, int32_t i, int32_t delta) {
    uint8_t* p = i ? rec(s, i) : nullptr;
    if (!p || (rd<int32_t>(p, 0x8a) == 0 && delta == -1)) return;
    for (int32_t k = rd<int32_t>(p, 0x60); k < (int32_t)p[0x3d] + rd<int32_t>(p, 0x60); k++) {
        uint8_t* c = rec(s, k);
        if (rd<int32_t>(c, 0x8a) == 0 && delta == -1) e->fatal(0x1dc);
        wr<int32_t>(c, 0x8a, rd<int32_t>(c, 0x8a) + delta);
    }
}

// 1.13c D2Client+0x5EA90 (ECX unused; need=param_2, twoPass=param_3, honourAge=param_4)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_5ea90(D2cRes5d* s, const D2cRes5dEnv* e,
                                                         int32_t need, int32_t twoPass, int32_t honourAge) {
    uint32_t target = (uint32_t)(s->used - need);
    while ((uint32_t)s->used > target) {
        bool freed = false;
        int pass = 0;
        void** n = s->list;
        for (;;) {
            if (n) {
                int32_t idx = *(int32_t*)n; // *piVar3
                uint8_t* r = rec(s, idx);
                if ((pass == 0 && (rd<int32_t>(r, 0x8a) != 0 || r[0x4c] != 0)) ||
                    (honourAge && (uint32_t)(s->now - rd<int32_t>(r, 0x7c)) < 0x2ee) ||
                    e->fn5cc10(idx)) {
                    n = (void**)n[1];
                    continue;
                }
                d2_client_5dd50(s, e, idx);
                freed = true;
            }
            if (!twoPass || ++pass > 1) break;
            n = s->list;
        }
        if (!freed) return 0;
    }
    return 1;
}

// 1.13c D2Client+0x5EB90
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t d2_client_5eb90(D2cRes5d* s, const D2cRes5dEnv* e) {
    if (!s->player) return 0;
    uint32_t a = e->ord10331(s->player);
    if (!a) return 0;
    a = e->ord10826(a);
    if (!a) return 0;
    return e->ord14byte(e->ord10014(a));
}
