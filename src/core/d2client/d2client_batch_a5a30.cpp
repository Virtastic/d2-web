// src/core/d2client/d2client_batch_a5a30.cpp -- twelve UNMAPPED 1.13c D2Client.dll unit-table leaves
// (alpha lane), from build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Unrecovered callees, ordinals and the
// register-carried arguments (EAX/ESI/EBX in the decompile) are explicit parameters / env seams.
// ponytail: retail unit/list links are 32-bit slots (+0xE4 next, +0x54 sub-ptr, node +0xC next); they are
// read zero-extended, valid only while the host keeps those objects below 4 GiB. Widen with the unit layout.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

typedef uintptr_t U;
static inline uint32_t& W(U base, uint32_t off) { return *(uint32_t*)(base + off); }
static inline U Lnk(U base, uint32_t off) { return (U)W(base, off); }

// Unit hash tables: 6 unit types x 0x80 buckets, +0x10A608 (DAT_6fbba608) and +0x109A08 (DAT_6fbb9a08).
static U g_tab_a[6 * 0x80];
static U g_tab_b[6 * 0x80];
static int32_t g_stamp;   // +0x123420 DAT_6fbd3420

struct D2ClientA5Env {
    void (*fatal)(uint32_t code);                                  // Ordinal_10265+10024+_exit(-1)
    void (*ord_10574)(); void (*ord_10032)(); void (*ord_10857)();
    U (*fn_9d630)(uint32_t a, uint32_t b);                         // 0x9D630
    U (*ord_10460)(U list); U (*ord_10464)(U node); uint32_t (*ord_10298)(U node);
    uint32_t (*fn_a52a0)(U owner);                                 // 0xA52A0
    void (*ord_10061)(U list); uint32_t (*ord_10202)(U unit, uint32_t, uint32_t, const char* file);
    void (*ord_11127)(); uint32_t (*ord_10646)(U list, U unit);
    void (*fn_a6ef0)(U row, uint32_t bucket, uint32_t id, uint32_t type);
    uint32_t (*fn_1510)(uint32_t a, uint32_t b);                   // 0x1510
    U (*ord_10688)(U p);
    U (*fn_a4e20)(uint32_t id, U row, uint32_t type);
    void (*fn_22b20)(U unit);
    int32_t (*ord_11044)(U a, uint32_t b); U stamp_src;            // DAT_6fbcc3b8
    void (*fn_25280)();
    void (*fn_6b300)(U unit, uint32_t a, uint32_t b, int32_t c, uint32_t d);
    void (*fn_88a70)(U unit, uint32_t a, uint32_t b, uint32_t c);
    U (*fn_51a80)();
    void (*fn_a6d60)(U unit);
    void (*fn_c7b00)(); void (*fn_c7e30)(int32_t a, int32_t b, int32_t c);
    void (*fn_53320)();
    U (*ord_10476)(); void (*ord_10561)();
    void (*fn_a7bf0)(uint32_t a, int32_t b, int32_t c, uint32_t d);
    const uint32_t* type_order;                                    // DAT_6fb8412c, 6 entries
};

// 1.13c D2Client+0xA5A30 (45 B): ESI = unit.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a5a30(U unit, const D2ClientA5Env* e) {
    if (unit) W(unit, 0xc4) |= 0x22;
    e->ord_10574(); e->ord_10032(); e->ord_10857();
}

// 1.13c D2Client+0xA5B60 (37 B): EAX = rec, ECX = arg. 0xFFFF = none.
extern "C" EMSCRIPTEN_KEEPALIVE uint16_t d2_client_a5b60(U rec, uint32_t arg, const D2ClientA5Env* e) {
    if (rec && W(rec, 0) == 0) {
        U r = e->fn_9d630(arg, W(rec, 12));
        return r ? *(uint16_t*)(r + 0x22) : 0xffff;
    }
    return 0xffff;
}

// 1.13c D2Client+0xA5C50 (309 B): EBX = owner, `flag` = stack arg.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a5c50(U owner, uint32_t flag, const D2ClientA5Env* e) {
    if (!owner) { e->fatal(0xa80); return; }
    U list = Lnk(owner, 0x60);
    if (!list) return;
    U cur = e->ord_10460(list);
    while (cur) {
        U next = e->ord_10464(cur);
        uint32_t h = e->ord_10298(cur);
        for (U p = g_tab_a[4 * 0x80 + (h & 0x7f)]; p; p = Lnk(p, 0xe4)) {
            if (W(p, 12) != h) continue;
            uint32_t code;
            if (W(p, 0) != 4) code = 0x5a;
            else if (!flag || e->fn_a52a0(owner) == 0) {
                if (W(owner, 0) == 0) e->ord_10061(list);
                if (!e->ord_10202(p, 0x100, 0x96, "..\\Source\\D2Client\\UNIT\\Item.cpp")) e->ord_11127();
                if (e->ord_10646(list, p)) {
                    e->fn_a6ef0((U)&g_tab_a[W(p, 0) * 0x80], W(p, 12) & 0x7f, W(p, 12), W(p, 0));
                    break;
                }
                code = 0xa92;
            } else {
                if (e->ord_10646(list, p)) break;
                code = 0xa96;
            }
            e->fatal(code); return;
        }
        cur = next;
    }
}

// 1.13c D2Client+0xA6180 (124 B): EAX = unit.
extern "C" EMSCRIPTEN_KEEPALIVE bool d2_client_a6180(U u, const D2ClientA5Env* e) {
    if (!u) return false;
    uint32_t v = W(u, 16);
    switch (W(u, 0)) {
    case 0: return v == 0x11;
    case 1: return e->fn_1510(1, 0x14) == 0 && v == 0xc;
    case 2: {
        U d = e->ord_10688(Lnk(u, 4));
        uint8_t f = *(uint8_t*)(d + 0x1b7);
        return (f & 2) || (v == 2 && (f & 1));
    }
    case 3: return (W(u, 0xc4) & 0x10000) != 0;
    case 4: return v == 3;
    }
    return false;
}

// 1.13c D2Client+0xA6210 (56 B): ESI = unit.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a6210(U u, const D2ClientA5Env* e) {
    uint32_t f = W(u, 0xc4) & 0xfffeffff;
    W(u, 0xc4) = f;
    if (W(u, 0) == 1 && e->fn_1510(f, 9) == 0) return;
    W(u, 0xc4) = f | 4;
}

// 1.13c D2Client+0xA6500 (310 B): ESI = unit. 1 = inserted, 0 = already present. Retail probes twice;
// the second hit (impossible single-threaded) is fatal.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a6500(U u, const D2ClientA5Env* e) {
    bool b = u && ((W(u, 0xc4) >> 0x15) & 1);
    U* tab = b ? g_tab_b : g_tab_a;
    uint32_t type = W(u, 0);
    U row = (U)&tab[type * 0x80];
    if (e->fn_a4e20(W(u, 12), row, type)) return 0;
    if (e->fn_a4e20(W(u, 12), row, type)) { e->fatal(b ? 0x9c : 0xa5); return 0; }
    e->fn_22b20(u);
    b = ((W(u, 0xc4) >> 0x15) & 1) != 0;   // fad2b20 may flip the table flag; retail re-reads it
    U* slots = (b ? g_tab_b : g_tab_a) + type * 0x80;
    uint32_t bk = W(u, 12) & 0x7f;
    U prev = 0, cur = slots[bk];
    while (cur && W(u, 12) < W(cur, 12)) { prev = cur; cur = Lnk(cur, 0xe4); }
    W(u, 0xe4) = (uint32_t)cur;   // ponytail: 32-bit link slot, see header
    if (!prev) slots[bk] = u; else W(prev, 0xe4) = (uint32_t)u;
    return 1;
}

// 1.13c D2Client+0xA6680 (174 B): when the stamp changes, run 25280 over every type-2 unit in both tables.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a6680(const D2ClientA5Env* e) {
    int32_t s = e->ord_11044(e->stamp_src, 0);
    if (s == g_stamp) return;
    g_stamp = s;
    for (int t = 0; t < 2; t++) {
        U* slots = (t ? g_tab_b : g_tab_a) + 2 * 0x80;
        for (int i = 0; i < 0x80; i++)
            for (U p = slots[i]; p;) {
                uint32_t ty = W(p, 0);
                p = Lnk(p, 0xe4);
                if (ty != 2) { e->fatal(t ? 0x896 : 0x88c); return; }
                e->fn_25280();
            }
    }
}

// 1.13c D2Client+0xA6F60 (90 B): EAX = amount, ECX = code, ESI = unit.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a6f60(U u, int32_t amt, int32_t code, const D2ClientA5Env* e) {
    if (!u) return;
    uint32_t k = 1;
    if (amt < 1) { amt = 0; k = 2; }
    e->fn_6b300(u, k, 0, amt, 0);
    if (code == 0x97 || code == 0x98) e->fn_88a70(u, 0, 0, 0);
}

// 1.13c D2Client+0xA6FC0 (55 B).
extern "C" EMSCRIPTEN_KEEPALIVE bool d2_client_a6fc0(U p, const D2ClientA5Env* e) {
    U u = e->fn_51a80();
    if (u && W(u, 0) == 2) {
        U d = e->ord_10688(Lnk(u, 4));
        if (*(int8_t*)(d + 0x167) < 0) return false;
    }
    return e->fn_51a80() == p;
}

// 1.13c D2Client+0xA7000 (178 B): free every unit in both tables, type_order sequence, via A6D60.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a7000(const D2ClientA5Env* e) {
    for (int t = 0; t < 2; t++)
        for (int k = 0; k < 6; k++) {
            U* slots = (t ? g_tab_b : g_tab_a) + e->type_order[k] * 0x80;
            for (int i = 0; i < 0x80; i++)
                while (slots[i]) {
                    U p = slots[i], n = Lnk(p, 0xe4);
                    e->fn_a6d60(p);
                    slots[i] = n;
                }
        }
}

// 1.13c D2Client+0xA70C0 (500 B): EAX = event id, ECX = unit.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a70c0(U u, int32_t ev, const D2ClientA5Env* e) {
    if (ev < 0x162) {
        if (ev > 0x15e) { e->fn_6b300(u, 4, 0, (u ? (int32_t)W(u, 4) : -1) - 0xb0, 0); return; }
        switch (ev) {
        case 0xf2: e->fn_6b300(u, 3, 0, 0, 0); return;
        case 0x14d: e->fn_6b300(u, 2, 0, 0, 0); return;
        case 0x154: e->fn_c7b00(); e->fn_c7e30(0, -0x20, -0x20); return;
        case 0x155: e->fn_c7b00(); e->fn_c7e30(0, 0x20, -0x20); return;
        case 0x156: e->fn_c7b00(); e->fn_c7e30(0, 0x20, 0x20); return;
        case 0x157: e->fn_c7b00(); e->fn_c7e30(0, -0x20, 0x20); return;
        }
    } else if (ev < 0x20a) {
        if (ev == 0x209) { e->fn_53320(); return; }
        if (ev == 0x196) {
            e->fn_c7b00(); e->fn_c7e30(-0x37, 0, 0);
            if (!u) return;
            U s = Lnk(u, 0x54);
            U d = s ? Lnk(s, 0x30) : 0;
            if (d) { W(d, 0x10) = 0; W(d, 0x14) = 0; W(d, 0x18) = 0x800; }
            if (d) { W(d, 0x28) = 0; W(d, 0x2c) = 0; W(d, 0x30) = 0x32; }
            if (d) W(d, 0) |= 0x20;
        } else if (ev > 0x1a0 && ev < 0x1a3 && u) e->fn_6b300(u, 2, 0, 0, 0);
    } else if (ev == 0x21f || ev == 0x2c0) e->fn_6b300(u, 3, 0, 0, 0);
}

// 1.13c D2Client+0xA7D00 (75 B): ESI = x offset, `dy` = stack local uStack_20 (y offset).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a7d00(int32_t dx, int32_t dy, const D2ClientA5Env* e) {
    for (U n = e->ord_10476(); n; n = Lnk(n, 0xc))
        if (W(n, 0x1c) & 1) {
            e->ord_10561();
            e->fn_a7bf0(W(n, 4), (int32_t)W(n, 8) + dx, (int32_t)W(n, 0x18) + dy, W(n, 0x14));
        }
}
