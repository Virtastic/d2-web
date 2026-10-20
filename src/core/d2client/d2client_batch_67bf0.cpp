// src/core/d2client/d2client_batch_67bf0.cpp -- 1.13c D2Client.dll text-list/scroll-window rows
// 0x67BF0..0x68B30, from build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane, no vectors).
// The list object is addressed as a byte buffer with retail 32-bit offsets (rd/wr below); calls into
// UNMAPPED callees and vtable slots go through D2ClientListEnv. Unverified choices are marked GUESS.
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <emscripten.h>

template <class T> static inline T rd(const uint8_t* b, uint32_t o) { T v; memcpy(&v, b + o, sizeof v); return v; }
template <class T> static inline void wr(uint8_t* b, uint32_t o, T v) { memcpy(b + o, &v, sizeof v); }

struct D2ClientListEnv {
    void (*refresh_88a70)();                          // D2Client+0x88A70(0,0,0,0)
    void (*update_67280)();                           // D2Client+0x67280
    int32_t (*hit_67360)(const uint8_t* self, const uint8_t* msg);
    int32_t (*hit_673a0)(const uint8_t* self, const uint8_t* msg);
    int32_t (*hit_67160)(const uint8_t* self);
    void (*vcall_1c)(const uint8_t* self, const uint8_t* msg); // vtable +0x1c
    void (*vcall_20)(const uint8_t* self, const uint8_t* msg); // vtable +0x20
    void (*callback)(const uint8_t* self);            // fn ptr at this+0x18074a
    void (*ctor_1cb0)(uint8_t* row);                  // D2Client+0x1CB0 (per-row Unicode init)
    int32_t (*ord_10031)();                           // Ordinal_10031
    void (*free_stat)(int32_t id, int32_t v);         // Ordinal_10043
};

// 1.13c D2Client+0x67BF0
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_67bf0(const uint8_t* b) {
    uint32_t n = rd<uint16_t>(b, 0x180748);
    int32_t acc = 0, i = 0;
    int32_t first = rd<int32_t>(b, 0x180730), rowh = rd<int32_t>(b, 0x180766), pad = rd<int32_t>(b, 0x18076a);
    int32_t lim = rd<int32_t>(b, 0x180714) - 0x14;
    for (; i < (int32_t)n; i++) {
        uint32_t p = 0x808 + (uint32_t)i * 0x300e; // psVar3
        if (first <= i) {
            static const int32_t sh[6] = {-0x400, 0, 0x400, 0x800, 0xc00, 0x1000};
            for (int k = 0; k < 6; k++)
                if (rd<int16_t>(b, p + sh[k] * 2)) acc += rowh;
            acc += pad;
        }
        if (lim < acc) break;
    }
    return i; // GUESS: retail returns unaff_EDI (decompiler lost the value); both exits identical
}

// 1.13c D2Client+0x67CC0 -- base destructor: only resets the vtable pointer (stores are dead here)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_67cc0(uint32_t* vtbl) { *vtbl = 0; /* GUESS: vptr is a host table id */ }

// 1.13c D2Client+0x67D00 -- list constructor; args param_2..param_9 map to the fields below
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t* d2_client_67d00(uint8_t* b, const D2ClientListEnv* env,
    uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t p7, uint32_t p8, uint32_t p9, char p10) {
    for (int i = 0; i < 0x80; i++) env->ctor_1cb0(b + 8 + (uint32_t)i * 0xC038);
    wr<uint32_t>(b, 0x180708, p2); wr<uint32_t>(b, 0x18070c, p3);
    wr<uint32_t>(b, 0x180710, p4); wr<uint32_t>(b, 0x180714, p5);
    wr<uint32_t>(b, 0x180740, p7);
    for (uint32_t o : {0x18071cu, 0x180728u, 0x180720u, 0x180724u, 0x180772u, 0x180730u}) wr<uint32_t>(b, o, 0);
    wr<uint32_t>(b, 0x180734, 0xffffffff); wr<uint32_t>(b, 0x180738, 0xffffffff);
    wr<uint32_t>(b, 0x180744, p8);
    wr<uint16_t>(b, 0x180748, 0);
    for (uint32_t i = 0; i < 0x80; i++) {
        uint32_t r = 0x808 + i * 0x300e;
        for (int k = -1; k < 6; k++) wr<uint16_t>(b, (uint32_t)((int32_t)r + k * 0x800), 0);
        wr<uint32_t>(b, r + 0x2806, 0); wr<uint32_t>(b, r + 0x280a, 1); wr<uint32_t>(b, r + 0x2802, 0);
    }
    wr<uint32_t>(b, 0x18074a, p9); wr<uint32_t>(b, 0x18076a, p6);
    wr<int32_t>(b, 0x18075a, rd<int32_t>(b, 0x180708) - 0x12 + rd<int32_t>(b, 0x180710));
    wr<int32_t>(b, 0x18075e, rd<int32_t>(b, 0x18070c) + 0x10);
    wr<int32_t>(b, 0x180766, 0xe);
    int32_t h = rd<int32_t>(b, 0x180714);
    wr<int32_t>(b, 0x180762, h < 0x20 ? 1 : (h - 0x10) / 0xc - 1);
    wr<int32_t>(b, 0x18074e, 1);
    wr<uint32_t>(b, 0x180718, env->ord_10031() == 2 ? (p10 ? 10u : 0u) + 0x12 : 0x12);
    return b;
}

// 1.13c D2Client+0x67F20 -- wrapper ctor: allocates 0x180776 bytes (Ordinal_401) and runs 0x67D00
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t* d2_client_67f20(uint8_t* wrapper, uint8_t* alloc, const D2ClientListEnv* env,
    uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, char p6) {
    // GUESS: the five stack args to 0x67D00 are decompiler-shifted; mapping here is (wrapper,p2..p5,p6) positional
    uint8_t* o = alloc ? d2_client_67d00(alloc, env, 0, 0, 0, p2, p3, p4, p5, 0, p6) : nullptr;
    wr<uint64_t>(wrapper, 8, (uint64_t)(uintptr_t)o);
    return wrapper;
}

static void clamp_scroll(uint8_t* b) {
    int32_t sel = rd<int32_t>(b, 0x180738), top = rd<int32_t>(b, 0x180730), vis = rd<int32_t>(b, 0x18074e);
    if (sel < top) wr<int32_t>(b, 0x180730, top = sel);
    if (vis + top <= sel) wr<int32_t>(b, 0x180730, top + 1);
}

// 1.13c D2Client+0x68440 -- select next row
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_68440(uint8_t* b, const D2ClientListEnv* env) {
    uint32_t n = rd<uint16_t>(b, 0x180748);
    if (rd<int32_t>(b, 0x18074a) == 0) {
        int32_t s = rd<int32_t>(b, 0x18074e) - 1 + rd<int32_t>(b, 0x180730);
        if ((int32_t)n <= s) s = (int32_t)n - 1;
        wr<int32_t>(b, 0x180738, s);
    }
    if (rd<int32_t>(b, 0x180738) < (int32_t)n - 1) {
        wr<int32_t>(b, 0x180738, rd<int32_t>(b, 0x180738) + 1);
        env->refresh_88a70();
        clamp_scroll(b);
    }
    return 1;
}

// 1.13c D2Client+0x684E0 -- select previous row
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_684e0(uint8_t* b, const D2ClientListEnv* env) {
    if (rd<int32_t>(b, 0x18074a) == 0) wr<int32_t>(b, 0x180738, rd<int32_t>(b, 0x180730));
    if (rd<int32_t>(b, 0x180738) > 0) {
        wr<int32_t>(b, 0x180738, rd<int32_t>(b, 0x180738) - 1);
        env->refresh_88a70();
        clamp_scroll(b);
    }
    return 1;
}

// 1.13c D2Client+0x68560 -- scrollbar-track click; msg y at +0xe (register-passed `this`/msg in retail)
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_68560(uint8_t* b, const uint8_t* msg, const D2ClientListEnv* env) {
    int32_t y0 = rd<int32_t>(b, 0x18075e), y = rd<uint16_t>(msg, 0xe);
    int32_t n = rd<uint16_t>(b, 0x180748), vis = rd<int32_t>(b, 0x18074e);
    if (!n || vis > n || y == y0 || y < y0) return 0;
    int32_t t0 = rd<int32_t>(b, 0x180752), t1 = rd<int32_t>(b, 0x180756);
    if (t0 == t1) return 0;
    int32_t sel = rd<int32_t>(b, 0x180738);
    if (y < y0 + 0xd + t0 * 0xc) {
        int32_t v = sel - vis;
        sel = v < 1 ? 0 : v;
    } else {
        if (y <= y0 + 0xd + t1 * 0xc) return 0;
        sel += vis;
        if (n <= sel) sel = n - 1;
        if (n - sel < vis) sel = n < vis ? 0 : n - vis;
    }
    if (sel > 0x7f) { sel = n - 1; if (sel < 0) sel = 0; }
    wr<int32_t>(b, 0x180738, sel);
    wr<int32_t>(b, 0x180730, sel);
    env->refresh_88a70();
    env->update_67280();
    return 1;
}

// 1.13c D2Client+0x68670 -- mouse-up dispatch
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_68670(uint8_t* b, const uint8_t* msg, const D2ClientListEnv* env) {
    if (rd<int32_t>(b, 0x18071c) == 0) { wr<int32_t>(b, 0x180734, -1); return 1; }
    wr<int32_t>(b, 0x18071c, 0);
    int32_t mx = rd<uint16_t>(msg, 0xc), my = rd<uint16_t>(msg, 0xe);
    if (rd<int32_t>(b, 0x180720) == 1) {
        wr<int32_t>(b, 0x180720, 0);
        int32_t x0 = rd<int32_t>(b, 0x18075a), y0 = rd<int32_t>(b, 0x18075e);
        if (x0 < mx && mx < x0 + 0x10 && y0 - 0xc < my && my < y0) env->vcall_1c(b, msg);
    } else if (rd<int32_t>(b, 0x180724) == 1) {
        wr<int32_t>(b, 0x180724, 0);
        if (env->hit_673a0(b, msg) == 1) { env->vcall_20(b, msg); wr<int32_t>(b, 0x180734, -1); return 0; }
    } else {
        if (rd<int32_t>(b, 0x180728) != 1) {
            if (rd<int32_t>(b, 0x18074a) == 0) return 0;
            int32_t h = env->hit_67160(b);
            if (rd<int32_t>(b, 0x180734) != h || h == -1) return 0;
            wr<int32_t>(b, 0x180734, -1);
            env->callback(b);
            return 1;
        }
        wr<int32_t>(b, 0x180728, 0);
        if (env->hit_67360(b, msg) == 1) {
            d2_client_68560(b, msg, env); // GUESS: retail passes msg in EAX
            wr<int32_t>(b, 0x180734, -1);
            return 0;
        }
    }
    wr<int32_t>(b, 0x180734, -1);
    return 0;
}

// 1.13c D2Client+0x68810 -- walk 0x200 hash buckets at +0xc; entry: next@0, flags@4, size@8
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_68810(const uint8_t* tbl, int32_t* mem_total_6fbcc044,
                                                    const D2ClientListEnv* env, uintptr_t (*next)(uintptr_t)) {
    (void)tbl; (void)mem_total_6fbcc044; (void)env; (void)next;
    // GUESS: bucket chain pointers are 32-bit retail addresses; walk is host-heap dependent, filed as seam
}

// 1.13c D2Client+0x689F0 -- radio-group exclusivity; ids and the +0x2c/+0x38 object lists are retail-shaped.
// Node: type@0 (8 = radio), +8 state, +0x38 group, +0x3c checked, next@0xa4.
struct D2ClientRadioNode { int32_t type, _p[1], st, _q[11], grp, chk; };
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_689f0(uint8_t* first, int32_t grp,
                                                    uint8_t* (*next)(const uint8_t*)) {
    // GUESS: the Ordinal_10494/10945/10625 lock+query block only gates +8 of EBX; omitted
    for (uint8_t* n = first; n; n = next(n))
        if (rd<int32_t>(n, 0x38) == grp && rd<int32_t>(n, 0) == 8) { wr<int32_t>(n, 0x3c, 0); wr<int32_t>(n, 8, -1); }
    int32_t pick = 0, mn = 0;
    for (uint8_t* n = first; n; n = next(n)) {
        if (rd<int32_t>(n, 0) != 8) continue;
        int32_t g = rd<int32_t>(n, 0x38);
        if ((pick == 0 || g < pick) && grp < g) pick = g;
        if (mn == 0 || g < mn) mn = g;
    }
    if (!pick) pick = mn;
    for (uint8_t* n = first; n; n = next(n))
        if (rd<int32_t>(n, 0x38) == pick && rd<int32_t>(n, 0) == 8) { wr<int32_t>(n, 0x3c, 1); wr<int32_t>(n, 8, 0); }
}

// 1.13c D2Client+0x68AE0 -- new radio node inherits group state
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_68ae0(uint8_t* self, uint8_t* first,
                                                    uint8_t* (*next)(const uint8_t*)) {
    for (uint8_t* n = first; n; n = next(n)) {
        if (rd<int32_t>(n, 0) != 8 || n == self) continue;
        if (rd<int32_t>(n, 0x38) == rd<int32_t>(self, 0x38)) {
            wr<int32_t>(self, 0x3c, rd<int32_t>(n, 0x3c)); wr<int32_t>(self, 8, rd<int32_t>(n, 8));
            return;
        }
        if (rd<int32_t>(n, 0x3c) != 0) { wr<int32_t>(self, 0x3c, 0); return; }
    }
    wr<int32_t>(self, 0x3c, 1); wr<int32_t>(self, 8, 0);
}

// 1.13c D2Client+0x68B30 -- path "...\name.ext" -> name (4 chars from end must be '.')
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_68b30(const char* path, char* out) {
    size_t n = strlen(path);
    if (n < 4 || path[n - 4] != '.') return;
    const char* p = path + n - 4;
    int32_t k = 0;
    while (*p != '\\') { p--; k++; }
    memcpy(out, p + 1, (size_t)(k - 1));
    out[k - 1] = 0;
}
