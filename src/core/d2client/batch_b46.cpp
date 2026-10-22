// D2Client batch b46-d2client-1: env-seam ports (callees/globals injected, no retail data).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;
static inline i32 rd(const void* p, u32 off) { i32 v; memcpy(&v, (const char*)p + off, 4); return v; }

// Shared tail of the 0x5B6C0/0x5B890 draw-record helpers.
struct D2cRecEnv {
    i32 (*f5B120)(u32* a, const void* b, u32* out);   // D2Client+0x5B120
    void (*f5B5A0)(i32 a, i32 b, i32 c);              // D2Client+0x5B5A0
    void (*f5AF10)(i32 a, i32 b);                     // D2Client+0x5AF10
    void (*ord10041)(const u32* rec, i32 off, i32 len, i32 m1, i32 mode, i32 z);
};
static i32 rec_common(const D2cRecEnv* e, const void* arg, i32* off, i32* y, i32* toggle) {
    u32 rec[18] = {0}, key;
    rec[13] = (u32)e->f5B120(nullptr, arg, &key);
    if (!rec[13]) return 0;
    e->f5B5A0(*off, (i32)key, *off);
    e->f5AF10(*off, *y);
    if (*toggle == 0) { *toggle = 10; *y += 10; } else { *y -= *toggle; *toggle = 0; }
    e->ord10041(rec, *off, 0x3c, -1, 1, 0);
    *off += 0x38;
    return 1;
}

// Float-select helpers 5DC70/5DCE0 and the unit-slot lookup they share.
static const uint8_t* slot(const uint8_t* tbl, i32 cnt, i32 i) {
    return (!tbl || i < 0 || i >= cnt) ? nullptr : tbl + i * 0x8e;
}

extern "C" {
// 1.13c D2Client+0x51470  (esi = state dword ptr, edi = latch ptr)
EMSCRIPTEN_KEEPALIVE i32 d2c_51470(i32 (*f557e0)(i32), i32 (*ord10143)(i32), const i32* esi, i32* edi,
    i32 a1, i32 a2, i32 a4, i32 g_cc3dc, i32 g_97ec, i32* g_9828) {
    if (!esi || *esi != 5) return 0;
    if (f557e0(a2) && f557e0(a4) && ord10143(a1) && g_cc3dc) {
        if (g_97ec < *g_9828) { *g_9828 = g_97ec + 500; return 1; }
        if (*edi == 0) { *g_9828 = g_97ec + 500; *edi = 1; }
    }
    return 0;
}

// 1.13c D2Client+0x51A80
EMSCRIPTEN_KEEPALIVE void* d2c_51a80(i32* g_c2f4, i32* g_c2f8, void* (*f54e20)(i32, i32, i32),
    i32 (*f568e0)(void*, i32, i32, i32), i32 g_9638, i32 slotbase, i32 g_964c) {
    if (!*g_c2f4) return nullptr;
    void* p = f54e20(g_9638, slotbase + g_964c * 0x80, g_964c);
    if (p && f568e0(p, 0, 0, 0)) return p;
    *g_c2f8 = 0; *g_c2f4 = 0;
    return nullptr;
}

// 1.13c D2Client+0x5B6C0  (esi=y, ecx/edx = param_1/arg)
EMSCRIPTEN_KEEPALIVE i32 d2c_5b6c0(const D2cRecEnv* e, const void* arg, i32* off, i32* toggle, i32* y) {
    return rec_common(e, arg, off, y, toggle);
}

// 1.13c D2Client+0x5B890  (body identical to 0x5B6C0)
EMSCRIPTEN_KEEPALIVE i32 d2c_5b890(const D2cRecEnv* e, const void* arg, i32* off, i32* toggle, i32* y) {
    return rec_common(e, arg, off, y, toggle);
}

// 1.13c D2Client+0x5B760  (eax=unit, ebx=kind; head = list head DAT_6fbcc4d4)
EMSCRIPTEN_KEEPALIVE i32 d2c_5b760(const D2cRecEnv* e, void (*f5ADD0)(i32), const void* unit, i32 kind,
    const void* head, i32* off, i32* y, i32* toggle) {
    i32 id = unit ? rd(unit, 0xc) : -1, n = 0;
    if (!head) return 0;
    for (const void* p = head; p; p = (const void*)(intptr_t)rd(p, 0x30))
        if (rd(p, 4) == kind && rd(p, 0xc) == id) n++;
    if (!n) return 0;
    u32 rec[18] = {0};
    u32 l50 = 0, l4c = 0;
    rec[13] = (u32)e->f5B120(&l50, unit, &l4c);
    if (!rec[13]) return 0;
    if (n == 1 && l50) {
        i32 v = -1;
        if (unit)
            for (const void* p = head; p; p = (const void*)(intptr_t)rd(p, 0x30))
                if (rd(p, 4) == kind && rd(p, 0xc) == rd(unit, 0xc) && rd(p, 0x20) == 0) { v = rd(p, 8); break; }
        e->f5B5A0(*off, v, *off);
    }
    f5ADD0(n);
    e->f5AF10(*off, *y);
    if (*toggle == 0) { *toggle = 10; *y += 10; } else { *y -= *toggle; *toggle = 0; }
    e->ord10041(rec, *off, 0x3c, -1, 1, 0);
    *off += 0x38;
    return 1;
}

// 1.13c D2Client+0x5B930
struct D2cB930Env {
    D2cRecEnv r;
    void (*f5B530)(i32 off, i32 y);
    u32 (*getTick)();
    void (*fatal)();                       // Ordinal_10265(0x5a)/Ordinal_10024 then _exit(-1)
    const void* const* bucket;             // DAT_6fbba808[128]
    i32 *g_c28c, *g_c290, *g_c294;
    i32 g_c298; u32 g_c2a0;
};
EMSCRIPTEN_KEEPALIVE i32 d2c_5b930(const D2cB930Env* e, const void* arg, i32* off, i32* y, i32* toggle) {
    u32 rec[18] = {0}, key;
    rec[13] = (u32)e->r.f5B120(nullptr, arg, &key);
    if (!rec[13]) return 0;
    e->r.f5B5A0(*off, (i32)key, *off);
    for (const void* p = e->bucket[key & 0x7f]; p; p = (const void*)(intptr_t)rd(p, 0xe4))
        if ((u32)rd(p, 0xc) == key) { if (rd(p, 0) != 1) e->fatal(); break; }
    e->f5B530(*off, *y);
    if (*toggle == 0) { *toggle = 10; *y += 10; } else { *y -= *toggle; *toggle = 0; }
    *e->g_c28c = *off; *e->g_c290 = *y;
    i32 mode;
    if (e->g_c298 == 0 || e->g_c2a0 == 0 || e->getTick() - e->g_c2a0 < 700) mode = *e->g_c294 ? 7 : 1;
    else { mode = 7; *e->g_c294 = 1; }
    e->r.ord10041(rec, *off, 0x3c, -1, mode, 0);
    *off += 0x38;
    return 1;
}

// 1.13c D2Client+0x5DC70  (returns float bits, no float in tree; eax = slot index; k = constants 0x6fb87544..54: [0]=44,[1]=48,[2]=4c,[3]=50,[4]=54)
EMSCRIPTEN_KEEPALIVE int32_t d2c_5dc70(const uint8_t* tbl, i32 cnt, i32 idx, const int32_t* k) {
    const uint8_t* s = slot(tbl, cnt, idx);
    i32 m = s ? rd(s, 0x47) : 1; // retail derefs address 0x47 on a null slot; we take the default arm
    switch (m) { case 0: return k[4]; case 2: return k[3]; case 3: return k[2]; case 4: return k[1]; default: return k[0]; }
}

// 1.13c D2Client+0x5DCE0  (k: [0]=58 default,[1]=5c,[2]=54,[3]=48)
EMSCRIPTEN_KEEPALIVE int32_t d2c_5dce0(const uint8_t* tbl, i32 cnt, i32 idx, const int32_t* k) {
    const uint8_t* s = slot(tbl, cnt, idx);
    i32 m = s ? rd(s, 0x47) : 0;
    switch (m) { case 2: return k[1]; case 3: return k[2]; case 4: return k[3]; default: return k[0]; }
}

// 1.13c D2Client+0x5EE70  (rng = 64-bit seed at rngobj+0x20)
EMSCRIPTEN_KEEPALIVE i32 d2c_5ee70(uint8_t* tbl, i32 cnt, i32 idx, uint8_t* rngobj, u32 (*f39640)()) {
    if (!tbl) return idx;
    const uint8_t* s = slot(tbl, cnt, idx);
    u32 b = s ? s[0x3d] : 0;
    if (b <= 1) return idx;
    i32 lim = (i32)b - 1; if (lim > 2) lim = 2;
    if (b < 4 && f39640() == 0) lim--;
    for (;;) {
        u32 r;
        if ((b & (b - 1)) == 0) {
            uint64_t sd; memcpy(&sd, rngobj + 0x20, 8);
            sd = (uint64_t)(u32)sd * 0x6ac690c5u + (u32)(sd >> 32);
            memcpy(rngobj + 0x20, &sd, 8); r = (b - 1) & (u32)sd;
        } else {
            uint64_t sd; memcpy(&sd, rngobj + 0x20, 8);
            sd = (uint64_t)(u32)sd * 0x6ac690c5u + (u32)(sd >> 32);
            memcpy(rngobj + 0x20, &sd, 8); r = (u32)sd % b;
        }
        i32 cand = (i32)r + idx;
        if (lim < 1) return cand;
        i32 i = 0;
        for (; i < lim; i++) if (cand == rd(s, 100 + i * 4)) break;
        if (i >= lim) return cand;
    }
}

// 1.13c D2Client+0x5F020
EMSCRIPTEN_KEEPALIVE void d2c_5f020(i32 g_c280, i32* g_c268, void (*f4f860)(), void (*f45660)(),
    void (*f5ed90)(i32), void (*f88dd0)(i32), void (*f5d080)(), void (*sleep0)()) {
    if (!g_c280) return;
    f4f860(); f45660(); f5ed90(*g_c268); f88dd0(*g_c268); f5d080();
    (*g_c268)++; sleep0();
}

// 1.13c D2Client+0x5F970  (eax = new value)
EMSCRIPTEN_KEEPALIVE u32 d2c_5f970(u32 v, i32 g_c1b0, i32* g_c208, i32* g_c20c,
    void (*ord426)(const char*, const char*, i32, u32)) {
    if (g_c1b0 == 1 && v == 1) { *g_c208 = 1; v = (*g_c20c != 0) - 1 & 2; }
    ord426("Diablo II", "AutoMapFade", 0, v);
    *g_c20c = v; *g_c208 = v;
    return v;
}

// 1.13c D2Client+0x5F9F0
struct D2cF9F0G { i32 *d3474, *d3470, *c240, *c23c, *c238, *c220, *c224, *c228, *c22c, *c230, *c234; i32 c1b0, c284, bc48, bc4c; };
EMSCRIPTEN_KEEPALIVE i32 d2c_5f9f0(const D2cF9F0G* g, i32 (*f108d0)(i32)) {
    if (*g->d3474 != (g->c284 != 2) && g->c1b0 == 1) { *g->d3474 = g->c284 != 2; return f108d0(1); }
    if (*g->d3470 != *g->c240 && g->c1b0 == 1) { *g->d3470 = *g->c240; return f108d0(1); }
    if (*g->c240 == 0) { *g->c23c = (g->bc48 * 2) / 3; *g->c238 = 0x4e; }
    else { *g->c23c = 0; *g->c238 = (g->c284 == 2) - 1 & 0x60; }
    *g->c220 = *g->c23c; *g->c224 = *g->c238;
    if (g->c1b0 != 1) { *g->c230 = g->bc48; *g->c228 = -16; *g->c22c = -16; *g->c234 = g->bc4c; return -16; }
    *g->c228 = *g->c23c - 8; *g->c230 = g->bc48 / 3 + *g->c23c;
    *g->c22c = *g->c238; *g->c234 = g->bc4c / 3 + *g->c238;
    return (i32)(((int64_t)g->bc4c * 0x55555556) >> 32);
}
}
