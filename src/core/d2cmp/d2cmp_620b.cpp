// 1.13c D2CMP+0x620B (_wctomb, VS2003 CRT): cdecl (mbch, wch). Retail: ptd = __getptd(); locale = ptd->ptlocinfo
// (+0x64); if it differs from the module ptlocinfo global (0x3276c) ___updatetlocinfo (0x67FE: __lock(0xC),
// ___updatetlocinfo_lk 0x673D, unlock) swaps the thread onto the global locale; then ___wctomb_mt(locale, mbch, wch).
// The TLS lookup is the caller's seam: ptd is passed in. Locale records keep retail's 32-bit layout.
#include <cstdint>

extern "C" int32_t d2cmp_61ab(uint32_t locale, char* mbch, uint16_t wch);

#define U32(p, o) (*(uint32_t*)((uintptr_t)(p) + (o)))

// 0x22718 __initiallocinfo (C locale; refcount 1, mb_cur_max 1, the two ctype counts 1) and 0x3276c, the module
// ptlocinfo global that points at it. +0x4c is the lc_time record whose +0xb4 is the refcount 0x673D steps.
static uint32_t g_lcTime[0xC0 / 4];
static uint32_t g_initialLocinfo[0x54 / 4] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 };
extern "C" uint32_t g_d2cmp_ptlocinfo = 0;   // 0x3276c; 0 = not yet pointed at __initiallocinfo
extern "C" uint32_t d2cmp_initial_locinfo() { return (uint32_t)(uintptr_t)g_initialLocinfo; }

static uint32_t ptlocinfo() {
    if (!g_d2cmp_ptlocinfo) { g_initialLocinfo[0x4c / 4] = (uint32_t)(uintptr_t)g_lcTime; g_d2cmp_ptlocinfo = d2cmp_initial_locinfo(); }
    return g_d2cmp_ptlocinfo;
}

// 1.13c D2CMP+0x673D ___updatetlocinfo_lk: release the thread's locale, take a reference on the global one.
static uint32_t d2cmp_673d(uint8_t* ptd) {
    const uint32_t g = ptlocinfo();
    const uint32_t old = U32(ptd, 0x64);
    if (old == g) return old;
    if (old) {
        U32(old, 0) -= 1;
        if (U32(old, 0x2c)) U32(U32(old, 0x2c), 0) -= 1;
        if (U32(old, 0x34)) U32(U32(old, 0x34), 0) -= 1;
        if (U32(old, 0x30)) U32(U32(old, 0x30), 0) -= 1;
        if (U32(old, 0x40)) U32(U32(old, 0x40), 0) -= 1;
        U32(U32(old, 0x4c), 0xb4) -= 1;
    }
    U32(ptd, 0x64) = g;
    U32(g, 0) += 1;
    if (U32(g, 0x2c)) U32(U32(g, 0x2c), 0) += 1;
    if (U32(g, 0x34)) U32(U32(g, 0x34), 0) += 1;
    if (U32(g, 0x30)) U32(U32(g, 0x30), 0) += 1;
    if (U32(g, 0x40)) U32(U32(g, 0x40), 0) += 1;
    U32(U32(g, 0x4c), 0xb4) += 1;
    // ponytail: ___freetlocinfo (old reaching refcount 0, not __initiallocinfo) is not ported; no caller drops the last reference
    return U32(ptd, 0x64);
}

// 1.13c D2CMP+0x620B
extern "C" int32_t d2cmp_620b(char* mbch, uint16_t wch, uint8_t* ptd) {
    uint32_t loc = U32(ptd, 0x64);
    if (loc != ptlocinfo()) loc = d2cmp_673d(ptd);
    return d2cmp_61ab(loc, mbch, wch);
}
