// src/core/d2client/d2client_batch_7368.cpp -- 1.13c D2Client+0x07368 = MSVC 2003 Release CRT
// __freefls@4, the per-thread data destructor (build-wasm/ghidra-d2client/decomp/0x7368.c). Base
// 0x6FAB0000; every RVA below is retail's, and every global this file owns is a static named at its
// retail RVA. Its callees (__lock, __unlock, _free, ___freetlocinfo) are host seams in retail's call
// order (the src/core/d2client/d2client_batch_b56.cpp pattern).
//
// Nothing records an earlier D2Lang/__freefls
// STALLED; that negative result was "nothing to vector" for a different DLL's row and does not apply
// to this D2Client row, which is implemented here.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// Callees this row reaches, address = retail RVA.
struct D2ClientBatch7368Env {
    void (*lock_)(int32_t);            // 0x4860 __lock
    void (*unlock_)(int32_t);          // 0x47AB __unlock; retail reaches it through 0x749A __unlock(0xD)
                                       // and 0x74A6 __unlock(0xC), its only two callers
    void (*free_)(void*);              // 0x4891 _free
    void (*freetlocinfo)(uint8_t*);    // 0x70F2 ___freetlocinfo
};

// +0x0DB63C  PTR_DAT_6fb8b63c: the CRT default locale (owned by d2client_batch_03f82.cpp).
extern uint32_t g_default_locale;
// +0x0DB5E8  DAT_6fb8b5e8: the CRT initial locale block (owned by d2client_batch_b56.cpp).
extern uint8_t g_initial_locale;
// +0x0DB7B8  DAT_6fb8b7b8: the CRT default per-thread block, compared by address at +0x54.
// Non-static: __mtinit (0x74DE) writes its address into the first _tiddata's +0x54.
uint8_t g_default_ptd;
// +0x123730  DAT_6fbd3730: the CRT default mbc-info block, compared by address at +0x60.
static uint8_t g_default_mbc;

// The per-thread block this function destroys: six owned pointers, a sentinel-compared pointer, the
// mbc-info refcount pointer at +0x60 and the thread locale pointer at +0x64 (the same _tlocinfo shape
// d2client_batch_b56.cpp's ___freetlocinfo_rv walks).
struct FreeflsPtd {
    uint8_t  head[0x24];
    uint32_t p24;      // 0x24
    uint8_t  pad28[4];
    uint32_t p2c;      // 0x2C
    uint8_t  pad30[4];
    uint32_t p34;      // 0x34
    uint8_t  pad38[4];
    uint32_t p3c;      // 0x3C
    uint8_t  pad40[4];
    uint32_t p44;      // 0x44
    uint32_t p48;      // 0x48
    uint8_t  gap[0x54 - 0x4C];
    uint32_t p54;      // 0x54
    uint8_t  pad58[8];
    uint32_t p60;      // 0x60
    uint32_t p64;      // 0x64
};
static_assert(offsetof(FreeflsPtd, p24) == 0x24, "ptd +0x24");
static_assert(offsetof(FreeflsPtd, p2c) == 0x2C, "ptd +0x2C");
static_assert(offsetof(FreeflsPtd, p34) == 0x34, "ptd +0x34");
static_assert(offsetof(FreeflsPtd, p3c) == 0x3C, "ptd +0x3C");
static_assert(offsetof(FreeflsPtd, p44) == 0x44, "ptd +0x44");
static_assert(offsetof(FreeflsPtd, p48) == 0x48, "ptd +0x48");
static_assert(offsetof(FreeflsPtd, p54) == 0x54, "ptd +0x54");
static_assert(offsetof(FreeflsPtd, p60) == 0x60, "ptd +0x60");
static_assert(offsetof(FreeflsPtd, p64) == 0x64, "ptd +0x64");

// 1.13c D2Client+0x07368 (__freefls@4): free the six pointers at +0x24..+0x48 and the one at +0x54
// when it is not the CRT default; under lock 0xD drop the mbc-info refcount at +0x60 and free it at zero
// when it is not the default; under lock 0xC drop the thread locale's refcount and those of its four
// tables and its lc_time's +0xB4 count, deep-free it at zero when it is neither the default nor the
// initial locale; then free the block itself. A NULL block is a no-op.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_7368(uint8_t* ptd, const D2ClientBatch7368Env* env) {
    if (ptd == nullptr) return;
    FreeflsPtd* p = (FreeflsPtd*)ptd;
    if (p->p24) env->free_((void*)(uintptr_t)p->p24);
    if (p->p2c) env->free_((void*)(uintptr_t)p->p2c);
    if (p->p34) env->free_((void*)(uintptr_t)p->p34);
    if (p->p3c) env->free_((void*)(uintptr_t)p->p3c);
    if (p->p44) env->free_((void*)(uintptr_t)p->p44);
    if (p->p48) env->free_((void*)(uintptr_t)p->p48);
    if (p->p54 != (uint32_t)(uintptr_t)&g_default_ptd)
        env->free_((void*)(uintptr_t)p->p54);

    env->lock_(0xD);
    uint32_t* mbc = (uint32_t*)(uintptr_t)p->p60;
    if (mbc != nullptr && ((*mbc -= 1), *mbc == 0) && mbc != (uint32_t*)(uintptr_t)&g_default_mbc)
        env->free_(mbc);
    env->unlock_(0xD);

    env->lock_(0xC);
    uint32_t* loc = (uint32_t*)(uintptr_t)p->p64;
    if (loc != nullptr) {
        *loc -= 1;
        if (loc[0xB]) *(uint32_t*)(uintptr_t)loc[0xB] -= 1;
        if (loc[0xD]) *(uint32_t*)(uintptr_t)loc[0xD] -= 1;
        if (loc[0xC]) *(uint32_t*)(uintptr_t)loc[0xC] -= 1;
        if (loc[0x10]) *(uint32_t*)(uintptr_t)loc[0x10] -= 1;
        *(uint32_t*)(uintptr_t)(loc[0x13] + 0xB4) -= 1;
        if (loc != (uint32_t*)(uintptr_t)g_default_locale &&
            loc != (uint32_t*)(uintptr_t)&g_initial_locale && *loc == 0)
            env->freetlocinfo((uint8_t*)loc);
    }
    env->unlock_(0xC);

    env->free_(ptd);
}
