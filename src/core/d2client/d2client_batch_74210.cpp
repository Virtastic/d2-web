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

// 1.13c D2Client+0x74AC0 (285 B): like 74610 but casts the message n times (n = ordinal-10786 result). Disassembly at 6FB24AC0:
// ECX = unit, EDX = p2 (skilldesc index), stack = p3 (ret 4). s40550: EAX = p2, stack word = unit (ret 4). o10786 (0xC316, stdcall 4):
// unit, row+0x138, p2, p3. sx/sy (0x1630/0x1660): ECX = unit. a1340: EBX = unit, EDI = &msg[7], stack word 0 = &msg[8]; its result alone
// decides (no test of the outputs). send (0xB30F0): ECX = &msg, called n times with msg[22] = 0..n-1.
// msg (23 dwords, zeroed): [0]=0x21 [1]=unit [4]=i [5]=x [6]=y [7],[8]=a1340 outs [11]=p2 [12]=p3 [21]=cb (0x6FB23B60).
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_74AC0(uint32_t unit, int32_t p2, uint32_t p3, uint32_t tbl, uint32_t cb_23b60,
                         int32_t (*s40550)(uint32_t eax, uint32_t unit), int32_t (*o10786)(uint32_t, uint32_t, int32_t, uint32_t),
                         int32_t (*sx)(uint32_t), int32_t (*sy)(uint32_t),
                         uint32_t (*a1340)(uint32_t ebx, uint32_t* edi_out, uint32_t* s0_out), fn_send send) {
    uint32_t r = rec(tbl, p2); if (!r) return 0;
    int32_t i = s40550((uint32_t)p2, unit);
    if (i < 0 || i >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    if (unit) wr32(unit + 0xc4, rd32(unit + 0xc4) | 0x40);
    int32_t n = o10786(unit, rd32(r + 0x138), p2, p3);
    uint32_t m[23] = {0};
    m[0] = 0x21; m[1] = unit;
    m[5] = (uint32_t)sx(unit);
    m[6] = (uint32_t)sy(unit);
    m[4] = (uint32_t)i; m[11] = (uint32_t)p2; m[12] = p3; m[21] = cb_23b60;
    if (!a1340(unit, &m[7], &m[8])) return 0;
    for (int32_t c = 0; c < n; c++) { m[22] = (uint32_t)c; send(m); }
    return 1;
}

// 1.13c D2Client+0x74BE0 (368 B): three ordinal-10786 lookups on a record; every p3-th step draws via 0xB9A80.
// Retail (ECX = unit, EDX = p2, stack = p3, ret 4), checked against the disassembly at 6FB24BE0:
//   s40550: EAX = p2, stack word = unit (ret 4); index must be in [0, tbl.b6c).
//   o10630 (0xC838): ECX = unit, EDX = p2; its result X (nonzero to go on) is the value 0xCB68 and 0xCB50 take.
//   o10786 (0xC316, stdcall 4): unit, row field, p2, p3; t1 (row+0x114), t2 (+0x118), t3 (+0x11C) must each be > 0.
//   o10980 (0xCB68, stdcall 1): c = o10980(X); o10243 (0xCB50, stdcall 2): (X, c + 1).
//   c % t1 != 0 returns 1 without the draw. s1330 (0x1330): EAX = n, ECX = unit + 0x20 (the seed); calls run in the order
//   2*t2+1, 1630(unit), 2*t2+1, 1660(unit), t3; dx = r1 - t2 + x, dy = r2 - t2 + y.
//   s69a80 (0xB9A80, stdcall 8, EAX = unit): (index, 0, 0, dx, dy, p2, p3, r4).
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_74BE0(uint32_t unit, int32_t p2, int32_t p3, uint32_t tbl,
                         int32_t (*s40550)(int32_t p2, uint32_t unit),
                         int32_t (*o10630)(uint32_t unit, int32_t p2),
                         int32_t (*o10786)(uint32_t, uint32_t, int32_t, int32_t), int32_t (*o10980)(uint32_t),
                         void (*o10243)(uint32_t, int32_t), uint32_t (*s1330)(uint32_t n, uint64_t* seed),
                         int32_t (*sx)(uint32_t), int32_t (*sy)(uint32_t),
                         void (*s69a80)(uint32_t unit, int32_t idx, int32_t z0, int32_t z1, int32_t dx, int32_t dy,
                                        int32_t p2, int32_t p3, uint32_t r4)) {
    uint32_t r = rec(tbl, p2); if (!r) return 0;
    int32_t i = s40550(p2, unit);
    if (i < 0 || i >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    int32_t X = o10630(unit, p2);
    if (!X) return 0;
    int32_t t1 = o10786(unit, rd32(r + 0x114), p2, p3); if (t1 <= 0) return 0;
    int32_t t2 = o10786(unit, rd32(r + 0x118), p2, p3); if (t2 <= 0) return 0;
    int32_t t3 = o10786(unit, rd32(r + 0x11c), p2, p3); if (t3 <= 0) return 0;
    int32_t c = o10980((uint32_t)X);
    o10243((uint32_t)X, c + 1);
    if (c % t1 != 0) return 1;
    if (unit + 0x20 == 0) return 0;
    uint64_t* k = (uint64_t*)(uintptr_t)(unit + 0x20);
    int32_t r1 = (int32_t)s1330(2 * t2 + 1, k);
    int32_t dx = r1 - t2 + sx(unit);
    int32_t r2 = (int32_t)s1330(2 * t2 + 1, k);
    int32_t dy = r2 - t2 + sy(unit);
    uint32_t r4 = s1330(t3, k);
    s69a80(unit, i, 0, 0, dx, dy, p2, p3, r4);
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

// 1.13c D2Client+0x74ED0 (102 B): thin dispatcher of mode 0..6 to a text draw. Disassembly at 6FB24ED0 (the decompile
// drops the case constants): the mode in ECX selects an id (0x103A 0x103B 0x103D 0x103E 0x103C 0x2771 0x2772 for cases 0..6,
// jump table at 6FB24F38); then 0xD1EC(ECX = id, stack: eax - [gB9A4], p1, 0) returns EAX; then 0xD37E(ECX = that, EDX = [gB9A0] + 0x24).
// Both callees are D2Client thunks (jmp [IAT]). The default (mode > 6) returns with no call. Stdcall, ret 4.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_74ED0(uint32_t this_ecx, int32_t eax, uint32_t p1, uint32_t g_bcb9a0, int32_t g_bcb9a4,
                         uint32_t (*o_d1ec)(uint32_t ecx, int32_t a0, uint32_t a1, uint32_t a2),
                         uint32_t (*o_d37e)(uint32_t ecx, uint32_t edx)) {
    static const uint32_t ids[7] = { 0x103a, 0x103b, 0x103d, 0x103e, 0x103c, 0x2771, 0x2772 };
    if (this_ecx > 6) return (uint32_t)eax;   // default: no call, EAX passes through
    uint32_t esi = g_bcb9a0 + 0x24;
    uint32_t r = o_d1ec(ids[this_ecx], (int32_t)((uint32_t)eax - (uint32_t)g_bcb9a4), p1, 0);
    return o_d37e(r, esi);                    // EAX on return is 0xD37E's result
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
