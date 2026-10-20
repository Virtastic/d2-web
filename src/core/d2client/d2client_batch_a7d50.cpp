// src/core/d2client/d2client_batch_a7d50.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (0xA7D50..0xA8880)
// reimplemented from the cached decompiles. Alpha batch lane: implementation + marker only, no spec,
// no vector. Register-passed args are named in retail order (eax, ecx, edx, then stack). Unrecovered
// callees and ordinals are host seams on D2ClientBatchA7D50Env; module statics sit at retail RVAs.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2ClientBatchA7D50Env {
    uint32_t (*ord10331)(void);                         // Ordinal_10331
    int32_t  (*ord10327)(void);                         // Ordinal_10327
    void     (*sub_197D0)(void);                        // 0x197D0 (FUN_6fac97d0)
    void     (*ord11103)(void);                         // Ordinal_11103
    void     (*ord10770)(int32_t);                      // Ordinal_10770
    void     (*ord10498)(int32_t);                      // Ordinal_10498
    int32_t  (*ord10057)(uint32_t);                     // Ordinal_10057
    void     (*sub_A6210)(void);                        // 0xA6210
    int32_t  (*ord11090)(void);                         // Ordinal_11090
    void     (*sub_6A220)(void);                        // 0x6A220
    void     (*sub_2A590)(uintptr_t arg, uintptr_t unit);   // 0x2A590
    void     (*sub_24490)(uintptr_t arg, uint32_t zero);    // 0x24490
    void     (*ord10043)(uint32_t, uint32_t);           // Ordinal_10043
    void     (*fatal)(uint32_t code);                   // Ordinal_10265 -> Ordinal_10024 -> _exit(-1)
    int32_t  (*ord10047)(void);                         // Ordinal_10047
    uint32_t (*tickCount)(void);                        // GetTickCount
    uint32_t (*sub_1400)(uint32_t* out);                // 0x1400 (returns value, out = uStack_8)
    int16_t  (*ord10482)(uint32_t, int32_t, int32_t, uint32_t);  // Ordinal_10482
};

static inline int32_t rd32(uintptr_t p, uint32_t off) { return *(const int32_t*)(p + off); }
static inline void wr32(uintptr_t p, uint32_t off, int32_t v) { *(int32_t*)(p + off) = v; }

static int32_t  g_lineA[4096];       // +0xCCDE8 DAT_6fbacde8
static int32_t  g_lineB[4096];       // +0xD0DE8 DAT_6fbb0de8
static int32_t  g_sign;              // +0xCCDE4 DAT_6fbacde4
static int32_t  g_frac;              // +0xD4DE8 DAT_6fbb4de8
static int32_t  g_dx;                // +0xD4DEC DAT_6fbb4dec
static int32_t  g_orgX, g_orgY;      // +0xD4DF0, +0xD4DF4
static int32_t  g_mode;              // +0xD5200 DAT_6fbb5200
static int32_t  g_cells[0x900 * 2];  // +0xD5208: 8-byte cells {dword0, dword1(colour)}, 0x30 per row
static uintptr_t g_listHead;         // +0xE5BEC DAT_6fbcbbec
static uint32_t g_listFreed;         // +0xE5BF4 _DAT_6fbcbbf4
static uint32_t g_unitId;            // +0xE5BFC DAT_6fbcbbfc
static int32_t  g_tickState;         // +0xFD41C DAT_6fbd341c
static uint32_t g_tickStamp;         // +0xFD418 DAT_6fbd3418
static int32_t  g_level;             // +0xE3808 DAT_6fbc9808
static int32_t  g_flag;              // +0xDD854 DAT_6fb8d854

extern "C" void d2_client_a8630(uintptr_t node, const D2ClientBatchA7D50Env* env);

// 1.13c D2Client+0xA7D50: unit (EDI) state transition; only for types 1/5 whose id resolves.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a7d50(uintptr_t unit, const D2ClientBatchA7D50Env* env) {
    if (unit == 0 || rd32(unit, 0) != 0) return;
    int32_t type = rd32(unit, 0x10);
    if (env->ord10331() == 0 || (type != 1 && type != 5)) return;
    if (env->ord10327() != 0) d2_client_a8630(unit, env);
    env->sub_197D0();
    env->ord11103();
    int32_t id = rd32(unit, 0x2c);
    if (rd32(unit, 0x10) == 0x13) env->ord10770(id);
    env->ord10498(id);
    int32_t r = env->ord10057(env->ord10331());
    uintptr_t arg = (uintptr_t)(((r != 0 ? 4u : 0u)) + 1u);
    env->sub_A6210();
    wr32(unit, 0xc4, (int32_t)(((uint32_t)rd32(unit, 0xc4) & 0xffffffbfu) | 2u));
    if (env->ord11090() != 0) {
        env->sub_6A220();
        env->sub_2A590(arg, unit);
        env->sub_24490(arg, 0);
    }
}

// 1.13c D2Client+0xA82B0: walk the list, releasing each node's two resources, then clear the head.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a82b0(const D2ClientBatchA7D50Env* env) {
    uintptr_t n = g_listHead;
    while (n != 0) {
        int32_t has = rd32(n, 0x30);
        n = (uintptr_t)(uint32_t)rd32(n, 0x28);
        if (has != 0) env->ord10043(0x57d, 0);
        env->ord10043(0x57f, 0);
    }
    g_listHead = 0;
}

// 1.13c D2Client+0xA8310: eax=row, ecx=col; copy one line-cache cell per the current step mode.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a8310(int32_t row, int32_t col) {
    int32_t d = row * 0x40 + col, a, b, i;
    switch (g_mode) {
    case 1:
        i = g_dx + d;
        if (g_lineB[i] == 0) g_lineA[d] = g_lineA[i]; else g_lineA[d] = g_lineB[i];
        return;
    case 2:
        i = d + g_dx;
        a = g_lineB[i] ? g_lineB[i] : g_lineA[i];
        i = (row + g_sign) * 0x40 + g_dx + col;
        b = g_lineB[i] ? g_lineB[i] : g_lineA[i];
        g_lineA[d] = ((0x100 - g_frac) * a + g_frac * b) >> 8;
        return;
    case 3:
        i = (g_sign + row) * 0x40 + col;
        a = g_lineB[i] ? g_lineB[i] : g_lineA[i];
        i = (g_sign + row) * 0x40 + g_dx + col;
        b = g_lineB[i] ? g_lineB[i] : g_lineA[i];
        g_lineA[d] = ((0x100 - g_frac) * a + g_frac * b) >> 8;
        return;
    case 4:
        i = (g_sign + row) * 0x40 + col;
        g_lineA[d] = g_lineB[i] ? g_lineB[i] : g_lineA[i];
        return;
    default:
        return;
    }
}

// 1.13c D2Client+0xA8470: eax=obj, edx=x, stack=y; pick step mode from the delta to obj+0x10/+0x14.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a8470(uintptr_t obj, int32_t x, int32_t y) {
    int32_t dx = rd32(obj, 0x10) - x, dy = rd32(obj, 0x14) - y;
    if (dx == 0 && dy == 0) { g_mode = 0; return; }
    if (dx < 0) { g_dx = -1; dx = -dx; } else g_dx = 1;
    if (dy < 0) { g_sign = -1; dy = -dy; } else g_sign = 1;
    if (dx == 0) { g_mode = 4; return; }
    if (dy == 0) { g_mode = 1; return; }
    if (dy <= dx) { g_mode = 2; g_frac = (dy << 8) / dx; return; }
    g_mode = 3; g_frac = (dx << 8) / dy;
}

// 1.13c D2Client+0xA8560: fill cells rows [r0..r1] x cols [c0..c1] with {0, colour}.
// eax=r1, ecx=r0, edx=colour, stack=c0, edi=c1.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a8560(int32_t r1, int32_t r0, uint32_t colour, int32_t c0, int32_t c1) {
    for (int32_t r = r0; r <= r1; ++r)
        for (int32_t c = c0; c <= c1; ++c) {
            int32_t* p = &g_cells[(r * 0x30 + c) * 2];
            p[0] = 0;
            p[1] = (int32_t)colour;
        }
}

// 1.13c D2Client+0xA85F0: eax=obj; null or type-2 obj is fatal.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a85f0(uintptr_t obj, const D2ClientBatchA7D50Env* env) {
    if (obj == 0) { env->fatal(0x18d); return; }
    wr32(obj, 0x20, 1);
    wr32(obj, 0x1c, 0);
    if (rd32(obj, 0xc) == 2) env->fatal(400);
}

// 1.13c D2Client+0xA8630: esi=node; unlink from the singly linked list, release, count.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a8630(uintptr_t node, const D2ClientBatchA7D50Env* env) {
    if (node == 0) return;
    uintptr_t prev = 0, cur = g_listHead;
    for (;;) {
        if (cur == 0) { env->fatal(0x17a); return; }
        if (cur == node) break;
        prev = cur;
        cur = (uintptr_t)(uint32_t)rd32(cur, 0x28);
    }
    if (prev == 0) g_listHead = (uintptr_t)(uint32_t)rd32(cur, 0x28);
    else wr32(prev, 0x28, rd32(cur, 0x28));
    if (rd32(node, 0x30) != 0) env->ord10043(0x181, 0);
    env->ord10043(0x183, 0);
    g_listFreed++;
}

// 1.13c D2Client+0xA86C0: eax=obj; store three colour bytes at +0x25..+0x27.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a86c0(uintptr_t obj, uint8_t a, uint8_t b, uint8_t c,
                                          const D2ClientBatchA7D50Env* env) {
    if (obj == 0) { env->fatal(0x167); return; }
    *(uint8_t*)(obj + 0x25) = a;
    *(uint8_t*)(obj + 0x26) = b;
    *(uint8_t*)(obj + 0x27) = c;
}

// 1.13c D2Client+0xA8710: eax=obj; return obj+0x18 >> 3.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_a8710(uintptr_t obj, const D2ClientBatchA7D50Env* env) {
    if (obj == 0) { env->fatal(0x155); return 0; }
    return rd32(obj, 0x18) >> 3;
}

// 1.13c D2Client+0xA8760: eax=a, param=b; octagonal distance approx (|max|*0x3d7 + |min|*0x197) >> 10.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a8760(int32_t a, int32_t b) {
    int32_t x = a < 0 ? -a : a, y = b < 0 ? -b : b;
    if (y > x) { int32_t t = x; x = y; y = t; }
    return (uint32_t)(x * 0x3d7 + y * 0x197) >> 10;
}

// 1.13c D2Client+0xA8800: esi passes through. Pick state 0/1/2 and latch it with a 2001 ms debounce.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_a8800(int32_t esi, const D2ClientBatchA7D50Env* env) {
    int32_t st = 2;
    bool ok = false;
    if (env->ord10047() == 0) ok = true;
    else if (g_flag != 0) { st = 1; ok = true; }
    if (ok && g_level > 9) {
        if (g_level > 0xc) {
            if (g_level > 0xf) goto latch;
            if (g_tickState > 0) return esi;
        }
        st = 1;
        goto latch;
    }
    st = 0;
latch:
    uint32_t t = g_tickStamp;
    int32_t s = g_tickState;
    if (st != g_tickState) {
        t = env->tickCount();
        s = st;
        if (t - g_tickStamp < 0x7d1) return esi;
    }
    g_tickState = s;
    g_tickStamp = t;
    return esi;
}

// 1.13c D2Client+0xA8880: probe the 0x30 x 0x30 window; mark cell dword0 = 1 where Ordinal_10482 hits.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a8880(const D2ClientBatchA7D50Env* env) {
    if (g_unitId == 0 || env->ord10331() == 0) return;
    int32_t base = 0;
    for (int32_t y = 0; base < 0x900; ++y, base += 0x30)
        for (int32_t x = 0; x < 0x30; ++x) {
            uint32_t o = 0;
            uint32_t v = env->sub_1400(&o);
            if (v == 0) v = env->ord10331();
            if (env->ord10482(v, g_orgX + x, g_orgY + y, 0x22) != 0) g_cells[(base + x) * 2] = 1;
        }
}
