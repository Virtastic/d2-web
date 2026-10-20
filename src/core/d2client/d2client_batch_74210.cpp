// src/core/d2client/d2client_batch_74210.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (alpha lane), from
// build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000. Globals are parameters; unrecovered callees
// and ordinals are host callbacks. A "msg" is the 0x17-dword zeroed record passed to 0xB30F0.
// GUESSED arguments (hidden registers / stack-frame overlaps): 74480 (EBX=unit), 74770 (EBX=bias int),
// 74930 (EAX=obj; ordinals 10786/10494/10542 argument-less in the decompile; stack ptr 0x..94 = msg,
// uStack_8 = extra), 74610/74AC0 (extra=uStack_c/stack word after msg), 74ED0 (EAX=value, ECX=this switch),
// 74FD0 (ECX unused). All rows stay NAMED-grade.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static inline int16_t rd16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }
static inline void wr32(uint32_t a, uint32_t v) { *(uint32_t*)(uintptr_t)a = v; }
typedef uint32_t (*fn_u)(uint32_t);
typedef uint32_t (*fn_send)(uint32_t* msg);

// sgptDataTables-relative record: returns ptr to 0x23c-byte entry or 0.
static uint32_t rec(uint32_t tbl, int32_t i) {
    if (i < 0 || i >= (int32_t)rd32(tbl + 0xba0)) return 0;
    return (uint32_t)i * 0x23c + rd32(tbl + 0xb98);
}

// 1.13c D2Client+0x74210 (309 B): spawn-type-0x20 message for a record's skill/state.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_74210(int32_t p1, uint32_t unit, int32_t p3, uint32_t p5, uint32_t p6, uint32_t tbl,
                         uint32_t mask4, uint32_t (*s60d0)(int), uint32_t (*s62a0)(uint32_t), uint32_t (*s1580)(void),
                         uint32_t (*s15b0)(void), int32_t (*sx)(uint32_t), int32_t (*sy)(uint32_t),
                         fn_send send, void (*s1b300)(uint32_t, int, int, int, int)) {
    if (p1 || !unit || !p3) return 0;
    uint32_t p = s60d0(0); if (!p) return 0;
    if (!s62a0(unit)) return 0;
    if (!(mask4 & *(const uint8_t*)(uintptr_t)(s1580() + 5))) return 0;
    uint32_t r = s15b0(); if (!r) return 0;
    int32_t i = rd16(r + 0xea);
    if (i < 0 || i >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    uint32_t m[23] = {0};
    m[0] = 0x20; m[1] = unit; m[2] = unit; m[4] = i; m[7] = sx(p); m[8] = sy(p) ; m[11] = p5; m[12] = p6;
    // ponytail: m[] offsets from the decompile's local_5c base (idx = (-0x5c - local)/4 reversed); verify with vector.
    send(m);
    int16_t s = rd16(r + 0x110);
    if (s >= 0 && s < (int32_t)rd32(tbl + 0xbc0)) s1b300(unit, 2, 0, 0, 0);
    return 1;
}

// 1.13c D2Client+0x74350 (303 B): type-1 message for record p2 on unit p1, then zero velocity of result.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_74350(uint32_t p1, int32_t p2, uint32_t p3, uint32_t tbl, uint32_t (*s5f70)(void),
                         int32_t (*sx)(uint32_t), int32_t (*sy)(uint32_t), uint32_t* (*send)(uint32_t*),
                         void (*s77e30)(int, int, int), void (*s301f0)(int), void (*s1b300)(uint32_t, int, int, int, int)) {
    uint32_t r = rec(tbl, p2); if (!r) return 0;
    int32_t i = rd16(r + 0xea);
    if (i < 0 || i >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    uint32_t p = s5f70();
    if (!p || !p1) return 0;
    uint32_t m[23] = {0};
    m[0] = 1; m[1] = p1; m[4] = i; m[7] = sx(p); m[8] = sy(p); m[11] = p3; m[9] = p2;
    uint32_t* o = send(m);
    if (o) {
        s77e30(0x118, 0, 0);
        if (o[0x15]) { uint32_t q = rd32(o[0x15] + 0x30); if (q) { wr32(q + 0x10, 0); wr32(q + 0x14, 0); wr32(q + 0x18, 0xfffec000); } }
    }
    s301f0(0);
    if (rd16(r + 0x10c) > 0) s1b300(p, 2, 0, 0, 0);
    return 1;
}

// 1.13c D2Client+0x74480 (393 B): EBX=unit (guess). Chooses record skill by state bit and sends type 0x21.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_74480(uint32_t unit_ebx, uint32_t tbl, uint32_t (*o10203)(void), uint32_t (*o10871)(void),
                     int32_t (*o10680)(uint32_t), uint32_t (*o10331)(void), int (*o10057)(uint32_t),
                     uint32_t (*s15b0)(void), uint32_t (*s1330)(uint64_t*), fn_send send) {
    if (!rd32(unit_ebx + 0x2c) || !o10203()) return;
    uint32_t a = o10871(); if (!a) return;
    int32_t n = o10680(0); o10680(a);
    if (n <= 0 || n >= (int32_t)rd32(tbl + 0xba0)) return;
    uint32_t r = s15b0(); if (!r) return;
    int32_t i = rd16(r + 0xea);
    if (i < 0 || i >= (int32_t)rd32(tbl + 0xb6c)) return;
    if (o10057(o10331())) return;
    uint32_t m[23] = {0}; m[0] = 0x21; m[4] = i;
    if (rd16(r + 0xec) >= 0) {
        uint64_t* k = (uint64_t*)(uintptr_t)(unit_ebx + 0x20);
        uint32_t b = s1330(k);
        if (rd16(r + 0xee) < 0) { if (!b) m[4] = rd16(r + 0xec); }
        else {
            if (b == 0) { m[4] = rd16(r + 0xec); send(m); return; }
            if (b == 1) { m[4] = rd16(r + 0xee); send(m); return; }
        }
    }
    send(m);
}

// 1.13c D2Client+0x74610 (350 B): flag unit |= 0x40, ordinal 10786, then send record message.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_74610(uint32_t unit, int32_t p2, uint32_t p3, uint32_t tbl, uint32_t extra_c,
                         int32_t (*s40550)(uint32_t), uint32_t (*o10786)(uint32_t, uint32_t, int32_t, uint32_t),
                         uint32_t (*s5f70)(void), int (*s51340)(int32_t*), uint32_t* (*send)(uint32_t*),
                         void (*o10637)(uint32_t*, uint32_t)) {
    uint32_t r = rec(tbl, p2); if (!r) return 0;
    int32_t i = s40550(unit);
    if (i < 0 || i >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    if (unit) wr32(unit + 0xc4, rd32(unit + 0xc4) | 0x40);
    uint32_t v = o10786(unit, rd32(r + 0x138), p2, p3);
    s5f70();
    uint32_t m[23] = {0};
    int32_t a[4] = {0};
    m[12] = extra_c;
    if (s51340(a) && a[0]) { uint32_t* o = send(m); if (o) { o10637(o, v); return 1; } }
    return 0;
}

// 1.13c D2Client+0x74770 (435 B): EBX=bias (guess). Sends two type-0x21 messages mirrored around the point, then type 1.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_74770(uint32_t p1, int32_t p2, int32_t bias_ebx, uint32_t tbl, uint32_t mask0,
                         int (*s51340)(int32_t*), int (*s51410)(uint32_t), uint32_t (*o10331)(uint32_t),
                         uint32_t (*s1400)(void), int (*o10057)(uint32_t), int32_t (*sx)(uint32_t),
                         int32_t (*sy)(uint32_t), fn_send send) {
    uint32_t r = rec(tbl, p2); if (!r) return 0;
    int32_t i = rd16(r + 0xea);
    if (i < 0 || i >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    uint32_t t = (uint32_t)i * 0x1a4 + rd32(tbl + 0xb64); if (!t) return 0;
    int32_t j = rd16(t + 0x18);
    if (j < 0 || j >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    int32_t l[4] = {0};
    if (!s51340(l) || !s51410(p1) || !o10331(p1)) return 0;
    uint32_t u = s1400();
    if (!(mask0 & *(const uint8_t*)(uintptr_t)((uint32_t)l[1] + 5)) && o10057(u)) return 0;
    uint32_t m[23] = {0};
    m[0] = 0x21; m[1] = r; m[2] = p2; m[3] = l[0]; m[9] = p1;
    int32_t y = sx(r), x = sy(r);
    m[6] = l[0] - (x - bias_ebx); m[7] = (y - l[0]) + bias_ebx; send(m);
    m[6] = (x - bias_ebx) + l[0]; m[7] = bias_ebx - (y - l[0]); send(m);
    m[2] = i; m[6] = 0; m[7] = 0; m[0] = 1; send(m);
    return 1;
}

// 1.13c D2Client+0x74930 (396 B): EAX=obj (guess). Builds a message for record p1, then fixes obj fields.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t* d2_client_74930(int32_t p1, uint32_t obj_eax, uint32_t tbl, uint32_t extra_8,
                          int (*s51340)(int32_t*), uint32_t (*s1330)(uint64_t*), int32_t (*o10786)(void),
                          uint32_t* (*send)(uint32_t*), void (*s3bc0)(void), int (*o10494)(void), void (*o10542)(void)) {
    uint32_t r = rec(tbl, p1); if (!r) return 0;
    int32_t e = rd16(r + 0xea);
    if (e < 0 || e >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    int32_t k = rd16(r + 0xec);
    if (k < 0 || k >= (int32_t)rd32(tbl + 0xb6c)) k = -1;
    int32_t l[8] = {0};
    if (!s51340(l)) return 0;
    if (k > -1) s1330((uint64_t*)(uintptr_t)(obj_eax + 0x20));
    int32_t n = o10786(); if (n < 1) n = 1;
    uint32_t m[23] = {0}; m[7] = p1; m[12] = extra_8; m[8] = n;
    uint32_t* o = send(m);
    if (o) s3bc0();
    if (o10494()) {
        if (rd32(obj_eax + 0x30)) { o10542(); wr32(obj_eax + 0x48, 0x500); return o; }
        wr32(obj_eax + 0x44, (rd32(obj_eax + 0x44) & 0xffffff00) - 0x200);
    }
    return o;
}

// 1.13c D2Client+0x74AC0 (285 B): like 74610 but sends the message ordinal-10786-count times with index.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_74AC0(uint32_t unit, int32_t p2, uint32_t p3, uint32_t tbl, uint32_t extra_c, uint32_t cb_23b60,
                         int32_t (*s40550)(uint32_t), int32_t (*o10786)(uint32_t, uint32_t, int32_t, uint32_t),
                         int (*s51340)(int32_t*), fn_send send) {
    uint32_t r = rec(tbl, p2); if (!r) return 0;
    int32_t i = s40550(unit);
    if (i < 0 || i >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    if (unit) wr32(unit + 0xc4, rd32(unit + 0xc4) | 0x40);
    int32_t n = o10786(unit, rd32(r + 0x138), p2, p3);
    uint32_t m[23] = {0}; int32_t a[3];
    m[12] = extra_c; m[22] = cb_23b60; m[9] = p2;
    if (!s51340(a)) return 0;
    for (int32_t c = 0; c < n; c++) { m[21] = c; send(m); }
    return 1;
}

// 1.13c D2Client+0x74BE0 (368 B): three ordinal-10786 lookups on a record; every p3-th step draws via 0xB9A80.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_74BE0(uint32_t unit, int32_t p2, int32_t p3, uint32_t tbl,
                         int32_t (*s40550)(uint32_t), int (*o10630)(void),
                         int32_t (*o10786)(uint32_t, uint32_t, int32_t, int32_t), int32_t (*o10980)(uint32_t),
                         void (*o10243)(uint32_t, int32_t), uint32_t (*s1330)(uint64_t*),
                         int32_t (*sx)(uint32_t), int32_t (*sy)(uint32_t),
                         void (*s69a80)(int32_t, uint32_t, int, int, int32_t, int32_t, int32_t, int32_t, uint32_t)) {
    uint32_t r = rec(tbl, p2); if (!r) return 0;
    int32_t i = s40550(unit);
    if (i < 0 || i >= (int32_t)rd32(tbl + 0xb6c) || !o10630()) return 0;
    if (o10786(unit, rd32(r + 0x114), p2, p3) <= 0) return 0;
    uint32_t a = rd32(r + 0x118);
    if (o10786(unit, a, p2, p3) <= 0) return 0;
    int32_t b = (int32_t)rd32(r + 0x11c);
    int32_t t = o10786(unit, b, p2, 0);
    if (t <= 0) return 0;
    int32_t c = o10980(a);
    o10243(a, c + 1);
    if (c % p3 == 0) {
        uint64_t* k = (uint64_t*)(uintptr_t)(unit + 0x20);
        int32_t dx = (int32_t)s1330(k) - b, x = sx(unit);
        int32_t dy = (int32_t)s1330(k) - b, y = sy(unit);
        s69a80(t, a, 0, 0, dx + x, dy + y, p2, t, s1330(k));
    }
    return 1;
}

// 1.13c D2Client+0x74D80 (212 B): free five handles (globals 6fbcbfdc..bfec) via ord 10065, string 0x7a when set.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_74D80(uint32_t* h_bcbfdc_to_bfec /*5 slots*/, void (*o10065)(uint32_t), void (*o10043)(int, int)) {
    for (int i = 0; i < 5; i++) {
        uint32_t v = h_bcbfdc_to_bfec[i];
        if (v) { o10065(v); o10043(0x7a, 0); h_bcbfdc_to_bfec[i] = 0; }
    }
}

// 1.13c D2Client+0x74ED0 (102 B): switch(this 0..6) all fall to the same draw: ord10003(eax - gB9A4, p1, 0); ord10150().
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_74ED0(uint32_t this_ecx, int32_t eax, uint32_t p1, int32_t g_bcb9a4,
                     void (*o10003)(int32_t, uint32_t, int), void (*o10150)(void)) {
    if (this_ecx > 6) return;
    o10003(eax - g_bcb9a4, p1, 0);
    o10150();
}

// 1.13c D2Client+0x74F90 (54 B): walk the list at global 6fbcbfd8 via next@+0xcd, string 0x108 each; then clear head.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_74F90(uint32_t* head_bcbfd8, void (*o10043)(int, int)) {
    uint32_t n = *head_bcbfd8;
    while (n) { n = rd32(n + 0xcd); o10043(0x108, 0); }
    *head_bcbfd8 = 0;
}

// 1.13c D2Client+0x74FD0 (84 B): 0x48-byte record {[0]=p5,[0x25]=p3(idx 10)} and ord10041(rec, g9a0+p4, p2-g9a4, -1, 5, 0).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_74FD0(int32_t p2, uint32_t p3, int32_t p4, uint32_t p5, int32_t g_bcb9a0, int32_t g_bcb9a4,
                     void (*o10041)(uint32_t*, int32_t, int32_t, int32_t, int, int)) {
    uint32_t m[18] = {0};
    m[0] = p5; m[13] = p3;
    o10041(m, g_bcb9a0 + p4, p2 - g_bcb9a4, -1, 5, 0);
}
