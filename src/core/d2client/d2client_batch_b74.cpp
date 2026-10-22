// src/core/d2client/d2client_batch_b74.cpp -- 1.13c D2Client.dll rows 0x987F0, 0x9CCE0, 0x9D6C0, 0x9E280
// from build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane, no vectors). Globals and callees are
// passed in through D2ClientB74Env; the 32-bit retail records are addressed as byte buffers.
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <emscripten.h>

template <class T> static inline T rd(const uint8_t* b, uint32_t o) { T v; memcpy(&v, b + o, sizeof v); return v; }
template <class T> static inline void wr(uint8_t* b, uint32_t o, T v) { memcpy(b + o, &v, sizeof v); }

struct D2ClientB74Env {
    uint8_t* (*find_9cad0)(int32_t kind);          // D2Client+0x9CAD0
    int32_t (*pick_4ee70)(int32_t v);              // D2Client+0x4EE70 (FUN_6fb0ee70)
    int32_t (*ord_10396)(int32_t v);               // Ordinal_10396
    uint8_t* (*find_9d630)(int32_t a, int32_t b);  // D2Client+0x9D630
    void (*fatal)(int32_t code);                   // Ordinal_10265 + Ordinal_10024 + _exit(-1)
    void (*unlink_free)(int32_t size, int32_t tag); // Ordinal_10043(size,0)
    void (*redo_9da70)(int32_t id);                // D2Client+0x9DA70
    void (*refresh_24f90)();                       // D2Client+0x24F90
    void (*visit_27170)(const uint8_t* node);      // D2Client+0x27170
};

// 1.13c D2Client+0x987F0 -- state flags in `g` (retail globals DAT_6fbcbe78, 6fbcbc34, 6fbaad94, 6fbcc434,
// 6fbd349c, 6fbd3498, 6fb8d5d0); returns 1 when the panel-close tail (FUN_6fb1f4a0 onward) ran.
struct D2ClientB74Close {
    uint32_t* e78; uint32_t* c34; uint32_t* ad94; uint32_t* cc434; uint32_t* d349c; uint32_t* d3498;
    uint32_t d5d0; uint32_t panel[9]; // DAT_6fbaadb0,addc,ade4,ade8,adec,adf0,adf4,adf8,ae00 in that order
    uint32_t ada8;
    void (*f1f4a0)(); void (*ecc60)(int32_t, int32_t); void (*eccc0)(int32_t, int32_t);
    int32_t (*ord_10048)(); void (*ord_10227)(); void (*fatal)(int32_t);
};
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_987f0(const D2ClientB74Close* c) {
    uint32_t old = *c->ad94;
    *c->e78 = 1;
    if (*c->c34 == 0xd) *c->c34 = 0xc;
    c->ord_10227();
    if (c->d5d0 != 0x26) c->fatal(0x139b);
    *c->ad94 = 0;
    if (!old) return 0;
    c->f1f4a0();
    bool idle = true;
    for (int i = 0; i < 9; i++) if (c->panel[i]) { idle = false; break; }
    if (idle) {
        *c->cc434 = 1;
        if (*c->d349c == 0) {
            *c->d349c = 1;
            if (*c->d3498) { c->ecc60(c->ord_10048(), 0); *c->d3498 = 0; } // GUESS: arg2 is the &DAT_6fba51ec string id
            c->eccc0(c->ord_10048(), 0);                                   // GUESS: arg2 is &DAT_6fba51bc
        }
    }
    if (c->ada8) c->eccc0(c->ord_10048(), 0);                              // GUESS: arg2 is &DAT_6fb80780
    return 1;
}

// 1.13c D2Client+0x9CCE0 -- pick a sound/entry id from record 9CAD0(kind); b = player record (seed at +0x20/+0x24)
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_9cce0(const D2ClientB74Env* e, uint8_t* b, uint32_t last_tick, int32_t kind, int32_t cc3b8) {
    uint8_t* r = e->find_9cad0(kind);
    if (!r) return 0;
    if (kind == 2) return rd<int32_t>(r, 12);
    auto lcg = [&]() { uint64_t s = (uint64_t)rd<uint32_t>(b, 0x20) * 0x6ac690c5u + rd<uint32_t>(b, 0x24); wr<uint64_t>(b, 0x20, s); return s; };
    int32_t v;
    uint32_t n = 0;
    do {
        v = rd<int32_t>(r, 0);
        if (kind == 0 && rd<int32_t>(r, 4) != 0 && (lcg() & 1) == 0) v = rd<int32_t>(r, 4);
        if (rd<int32_t>(r, 8) != 0 && (v == 0 || (int32_t)((uint32_t)lcg() % 3) == 0)) {
            int32_t t = e->ord_10396(cc3b8);
            v = rd<int32_t>(r, 8) + (t == 1 ? 0 : (t < 2 || 3 < t) ? 2 : 1);
        }
        v = e->pick_4ee70(v);
    } while (v == rd<int32_t>(r, 16) && ++n < 0x14);
    wr<int32_t>(r, 16, v);
    wr<uint32_t>(r, 20, last_tick); // DAT_6fbc97f8
    return v;
}

// 1.13c D2Client+0x9D6C0 -- true when both ids resolve to records sharing one valid (!= -1) +0x22 group
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_9d6c0(const D2ClientB74Env* e, int32_t a, int32_t b, int32_t c) {
    const uint8_t* x = e->find_9d630(a, b);
    const uint8_t* y = e->find_9d630(a, c); // GUESS: ECX passed through unchanged (extraout_ECX)
    return x && y && rd<int16_t>(x, 0x22) == rd<int16_t>(y, 0x22) && rd<int16_t>(x, 0x22) != -1;
}

// 1.13c D2Client+0x9E280 -- unlink record `id` from the list at *head (next at +0x80, id at +0x10), free its
// sub-lists, re-run 9DA70, then walk the list through 27170. Returns the new head.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_9e280(const D2ClientB74Env* e, uint8_t* const* nodes, uint32_t n, uint32_t* head, int32_t id) {
    // nodes[i] stands for the retail node at address (i+1) in `*head`-space; the host maps links by index.
    if (id == -1) e->fatal(0x11e);
    // ponytail: list linkage lives in host memory; only the control flow is recovered here.
    uint32_t prev = 0, cur = *head;
    while (cur) {
        const uint8_t* nd = nodes[cur - 1];
        if (rd<int32_t>(nd, 0x10) == id) {
            uint32_t next = rd<uint32_t>(nd, 0x80);
            if (!prev) *head = next; else wr<uint32_t>(nodes[prev - 1], 0x80, next);
            if (rd<uint32_t>(nd, 0x34)) e->unlink_free(0x40, 0);
            wr<uint32_t>(const_cast<uint8_t*>(nd), 0x34, 0);
            wr<uint32_t>(const_cast<uint8_t*>(nd), 0x38, 0);
            e->unlink_free(0x12e, 0);
            break;
        }
        prev = cur; cur = rd<uint32_t>(nd, 0x80);
    }
    e->redo_9da70(id);
    e->refresh_24f90();
    uint32_t it = *head ? rd<uint32_t>(nodes[*head - 1], 0x80) : 0;
    for (int32_t k = 0; it || k > 8;) {
        e->visit_27170(it ? nodes[it - 1] : nullptr);
        k++;
        it = it ? rd<uint32_t>(nodes[it - 1], 0x80) : *head;
        if (k > 64) break; // GUESS: retail loop can spin when head==0; bounded here
    }
    (void)n;
    return *head;
}
