// src/core/d2client/d2client_batch_71360.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (alpha lane),
// from build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Globals are parameters; ordinal imports and
// unrecovered callees are host callbacks named ordN / sub_X after the retail arguments.
// GUESSES (hidden registers): 724E0 EAX=unit; 72830/728B0 EAX=unit-ish (in_EAX) and EBX (unaff_EBX).
#include <cstdint>
#include <initializer_list>
#include <emscripten.h>

static inline int16_t rs16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }
static inline uint32_t r32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }

// 1.13c D2Client+0x71360: free three ordinal-10065 handles (c000, c008, c004), then 0xBE020.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_71360(uint32_t* h_c000, uint32_t* h_c008, uint32_t* h_c004,
                     void (*ord10065)(uint32_t), void (*ord10043)(int, int), void (*sub_be020)(void)) {
    uint32_t* h[3] = {h_c000, h_c008, h_c004};
    for (auto p : h) { uint32_t v = *p; if (v) { ord10065(v); ord10043(0x7a, 0); *p = 0; } }
    sub_be020();
}

// 1.13c D2Client+0x71740
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_71740(int32_t b9a4, void (*ord10184)(void), void (*ord10003)(void),
                     void (*ord10028)(void), void (*ord10150)(int, int, int)) {
    ord10184(); ord10003();
    int32_t x = 0x136 - b9a4;
    ord10028(); ord10150(x, 0, 0); ord10184();
}

// 1.13c D2Client+0x717B0
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_717b0(int32_t b9a4, void (*ord10184)(void), void (*ord10003)(void),
                     void (*ord10028)(void), void (*ord10150)(int, int, int), void (*ord10227)(void)) {
    ord10184(); ord10003();
    int32_t x = 0xbe - b9a4;
    ord10028(); ord10150(x, 0, 0);
    ord10227(); ord10003();
    x = 0xcd - b9a4;
    ord10028(); ord10150(x, 0, 0); ord10184();
}

// 1.13c D2Client+0x72040
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_72040(uint32_t p1, uint32_t p2, uint32_t (*ord10871)(uint32_t, uint32_t),
                     uint32_t (*ord10680)(uint32_t, int, int), void (*ord10069)(uint32_t, uint32_t, uint32_t)) {
    uint32_t u = ord10871(p1, p2);
    if (!u) return;
    uint32_t a = ord10680(u, 0x161, 0);
    if (a < 6) ord10069(p1, a, ord10680(u, 0x162, 0));
}

// 1.13c D2Client+0x724E0: unit in EAX (guess); type 0 (or null -> 6 -> false) mode 0x12, type 1 mode 0xe.
extern "C" EMSCRIPTEN_KEEPALIVE
int d2_client_724e0(uint32_t unit_eax) {
    if (!unit_eax) return 0;
    uint32_t t = r32(unit_eax), mode = r32(unit_eax + 0x10);
    return t == 0 ? mode == 0x12 : t == 1 ? mode == 0xe : 0;
}

// 1.13c D2Client+0x727F0
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_727f0(int32_t id, int32_t (*ord10799)(int32_t), void (*sub_69660)(int32_t)) {
    if (id == -1) return;
    int32_t n = ord10799(id);
    for (int32_t i = 0; i < n; ++i) sub_69660(i + id);
}

static void skill_tail(uint32_t unit, uint32_t ebx, uint32_t dt_b98, int32_t dt_ba0, int line,
                       int32_t (*ord10601)(uint32_t, const char*, int), uint32_t (*ord10397)(uint32_t),
                       void (*sub_2b960)(uint32_t, uint32_t), void (*sub_75e50)(uint32_t), uint32_t in_eax) {
    int32_t i = ord10601(unit, "..\\Source\\D2Client\\UNIT\\PlrSkills.cpp", line);
    if (i < 0 || i >= dt_ba0) return;
    uint32_t rec = dt_b98 + (uint32_t)i * 0x23c;
    int16_t k = rs16(rec + 0x2e);
    if (k == 0x43) { if (ord10397(unit) & 1) sub_2b960(in_eax, ebx); }
    else if (k == 0x4c) sub_75e50(ebx);
}

// 1.13c D2Client+0x72830 (line 0x4f)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_72830(uint32_t in_eax, uint32_t ebx, uint32_t dt_b98, int32_t dt_ba0,
                     uint32_t (*ord10511)(void), int (*sub_724e0)(uint32_t),
                     int32_t (*ord10601)(uint32_t, const char*, int), uint32_t (*ord10397)(uint32_t),
                     void (*sub_2b960)(uint32_t, uint32_t), void (*sub_75e50)(uint32_t)) {
    uint32_t u = ord10511();
    if (u && sub_724e0(in_eax)) skill_tail(u, ebx, dt_b98, dt_ba0, 0x4f, ord10601, ord10397, sub_2b960, sub_75e50, in_eax);
}

// 1.13c D2Client+0x728B0 (line 0x26)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_728b0(uint32_t in_eax, uint32_t ebx, uint32_t dt_b98, int32_t dt_ba0,
                     uint32_t (*ord10511)(void), int (*sub_724e0)(uint32_t),
                     int32_t (*ord10601)(uint32_t, const char*, int), uint32_t (*ord10397)(uint32_t),
                     void (*sub_2b960)(uint32_t, uint32_t), void (*sub_75e50)(uint32_t)) {
    uint32_t u = ord10511();
    if (u && sub_724e0(in_eax)) skill_tail(u, ebx, dt_b98, dt_ba0, 0x26, ord10601, ord10397, sub_2b960, sub_75e50, in_eax);
}

static inline int32_t cl(uint32_t rec, unsigned o) { int16_t v = rs16(rec + o); return v < 0 ? -1 : v; }

// 1.13c D2Client+0x729A0: record = dt_bc + idx*0x3c, count dt_c4.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_729a0(int32_t idx, uint32_t dt_bc, int32_t dt_c4, void (*sub_727f0)(int32_t)) {
    if (idx < 0 || idx >= dt_c4) return;
    uint32_t r = dt_bc + (uint32_t)idx * 0x3c;
    int32_t v[3] = {cl(r, 2), cl(r, 4), cl(r, 6)};
    for (int32_t x : v) if (x != -1) { sub_727f0(x); sub_727f0(x + 1); }
}

// 1.13c D2Client+0x72A50 (retail passes the same id three times for +10/+2/+4; kept)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_72a50(int32_t idx, uint32_t dt_bc, int32_t dt_c4, void (*sub_727f0)(int32_t)) {
    if (idx < 0 || idx >= dt_c4) return;
    uint32_t r = dt_bc + (uint32_t)idx * 0x3c;
    int32_t a = cl(r, 10), b = cl(r, 2), c = cl(r, 4), d = cl(r, 6), e = cl(r, 8);
    for (int32_t x : {a, b, c}) if (x != -1) { sub_727f0(x); sub_727f0(x); sub_727f0(x); }
    if (d != -1) sub_727f0(d);
    if (e != -1) sub_727f0(e);
}

// 1.13c D2Client+0x72BB0
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_72bb0(int32_t p1, uint32_t p2, uint32_t dt_b98, int32_t dt_ba0,
                     uint32_t (*ord10871)(int32_t, uint32_t), int32_t (*ord10680)(uint32_t, int, int),
                     uint32_t (*sub_225d0)(int32_t), void (*sub_88c20)(int32_t, int)) {
    uint32_t u = ord10871(p1, p2);
    if (!u) return;
    int32_t i = ord10680(u, 0x15e, 0);
    if (i < 0 || i >= dt_ba0) return;
    if (rs16(dt_b98 + (uint32_t)i * 0x23c + 0x10e) > 0 && sub_225d0(p1)) sub_88c20(p1, 1);
}

// 1.13c D2Client+0x72C20
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_72c20(int32_t p1, uint32_t p2, uint32_t dt_b98, int32_t dt_ba0,
                     uint32_t (*ord10871)(int32_t, uint32_t), int32_t (*ord10680)(uint32_t, int, int),
                     void (*sub_88a70)(int32_t, int, int, int)) {
    uint32_t u = ord10871(p1, p2);
    if (!u) return;
    int32_t i = ord10680(u, 0x15e, 0);
    if (i < 0 || i >= dt_ba0) return;
    if (rs16(dt_b98 + (uint32_t)i * 0x23c + 0x10e) > 0) sub_88a70(p1, 0, 0, 0);
}
