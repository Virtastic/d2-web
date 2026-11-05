// src/core/d2client/d2client_batch_74210.cpp -- UNMAPPED 1.13c D2Client.dll rows 74AC0 and up (alpha lane), from
// build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000. Globals are parameters; unrecovered callees
// and ordinals are host callbacks. A "msg" is the 0x17-dword zeroed record passed to 0xB30F0.
// 74210..74930 live in their own d2client_<rva>.cpp files.
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

// old form of D2Client 74F90 (now d2client_74f90.cpp) (54 B): walk the list at global 6fbcbfd8 via next@+0xcd, string 0x108 each; then clear head.
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
