// src/core/d2win/d2win_batch_crt_22f8.cpp -- UNMAPPED 1.13c D2Win.dll VC7.1 CRT rows (lock table,
// atexit table) transcribed from the cached decompiles. Alpha batch lane: no spec, no vector.
// Not ported (compiler artifacts, no C++ equivalent): D2Win+0x25F8 __SEH_prolog and +0x2633
// __SEH_epilog (raw EBP/FS:[0] frame manipulation); skipped.
#include <cstdint>
#include <cstdlib>
#include <cerrno>
#include <malloc.h>
#include <emscripten.h>
#include <Windows.h>

extern "C" uint32_t d2win_g_6f8ff050[0x24 * 2];   // lock table, defined in d2win_batch_tiny_1018.cpp
extern "C" void d2win_sub_2396(int32_t lock_num);  // __unlock
extern "C" void d2win_sub_10A7(void);              // _lockexit
extern "C" void d2win_sub_2442(void);              // unlock(10)
extern "C" __attribute__((weak)) void d2win_sub_1962(int32_t code);  // __amsg_exit, own row

// Static storage standing in for the image's lock blocks at 0x6F901040 (0x18 B each).
static CRITICAL_SECTION g_cs[0x24];

// 1.13c D2Win+0x22F8 (__mtinitlocks): give every slot flagged static (second dword == 1) its lock block
// and initialise it (spin count 4000 is host-side, a no-op here). Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_22F8(void) {
    for (int i = 0; i < 0x24; i++)
        if (d2win_g_6f8ff050[i * 2 + 1] == 1) {
            d2win_g_6f8ff050[i * 2] = (uint32_t)(uintptr_t)&g_cs[i];
            InitializeCriticalSection(&g_cs[i]);
        }
    return 1;
}

// 1.13c D2Win+0x2341 (__mtdeletelocks): delete and free every dynamically allocated lock (flag != 1),
// then delete the static ones (flag == 1; not freed, not cleared).
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_2341(void) {
    for (int i = 0; i < 0x24; i++) {
        uint32_t p = d2win_g_6f8ff050[i * 2];
        if (p && d2win_g_6f8ff050[i * 2 + 1] != 1) {
            DeleteCriticalSection((LPCRITICAL_SECTION)(uintptr_t)p);
            free((void *)(uintptr_t)p);
            d2win_g_6f8ff050[i * 2] = 0;
        }
    }
    for (int i = 0; i < 0x24; i++)
        if (d2win_g_6f8ff050[i * 2] && d2win_g_6f8ff050[i * 2 + 1] == 1)
            DeleteCriticalSection((LPCRITICAL_SECTION)(uintptr_t)d2win_g_6f8ff050[i * 2]);
}

extern "C" void d2win_sub_244B(int32_t lock_num);

// 1.13c D2Win+0x23ab (__mtinitlocknum): lazily allocate lock block `lock_num` (0x18 B) under lock 10.
// Returns 1, or 0 with errno = ENOMEM(12) on allocation failure.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_23AB(int32_t lock_num) {
    uint32_t *slot = &d2win_g_6f8ff050[lock_num * 2];
    if (*slot) return 1;
    void *blk = malloc(0x18);
    if (!blk) { errno = 12; return 0; }
    d2win_sub_244B(10);
    if (*slot == 0) {
        InitializeCriticalSection((LPCRITICAL_SECTION)blk);
        *slot = (uint32_t)(uintptr_t)blk;
    } else free(blk);
    d2win_sub_2442();
    return 1;
}

// 1.13c D2Win+0x0244b (__lock): create the slot's lock on first use (fatal _amsg_exit(0x11) if that
// fails), then enter it.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_244B(int32_t lock_num) {
    if (d2win_g_6f8ff050[lock_num * 2] == 0 && d2win_sub_23AB(lock_num) == 0 && d2win_sub_1962)
        d2win_sub_1962(0x11);
    EnterCriticalSection((LPCRITICAL_SECTION)(uintptr_t)d2win_g_6f8ff050[lock_num * 2]);
}

// atexit table: DAT_6f9abc50 base, DAT_6f9abc4c next free slot (host pointers here).
static uintptr_t *g_onexit_base, *g_onexit_next;

// 1.13c D2Win+0x24FC (___onexitinit): allocate the 0x80-byte table, first slot 0. Returns 0, or 0x18.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_24FC(void) {
    g_onexit_base = (uintptr_t *)malloc(0x80);
    if (!g_onexit_base) return 0x18;
    *g_onexit_base = 0;
    g_onexit_next = g_onexit_base;
    return 0;
}

// 1.13c D2Win+0x247C (__onexit_lk): append `func`; when the table is full grow it by min(size,0x800),
// retrying at size+0x10, and drop the entry if both reallocs fail.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_247C(uintptr_t func) {
    size_t sz = malloc_usable_size(g_onexit_base);
    if (sz < (size_t)((char *)g_onexit_next - (char *)g_onexit_base) + 4) {
        size_t grow = sz < 0x800 ? sz : 0x800;
        uintptr_t *n = (uintptr_t *)realloc(g_onexit_base, sz + grow);
        if (!n && !(n = (uintptr_t *)realloc(g_onexit_base, sz + 0x10))) return;
        g_onexit_next = n + (g_onexit_next - g_onexit_base);
        g_onexit_base = n;
    }
    *g_onexit_next++ = func;
}

// 1.13c D2Win+0x2556: _unlockexit thunk.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_2556(void) { d2win_sub_2396(8); }

// 1.13c D2Win+0x2524 (__onexit): under the exit lock, register `func`; returns func.
extern "C" EMSCRIPTEN_KEEPALIVE uintptr_t d2win_sub_2524(uintptr_t func) {
    d2win_sub_10A7();
    d2win_sub_247C(func);
    d2win_sub_2556();
    return func;
}

// 1.13c D2Win+0x255C (_atexit): 0 on success, -1 if registration returned null.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_255C(uintptr_t func) {
    return d2win_sub_2524(func) != 0 ? 0 : -1;
}

// 1.13c D2Win+0x256E (__RTC_Initialize): its callback range [0x6F8FD244, 0x6F8FD244) is empty, so the
// loop never runs; nothing to do.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_256E(void) {}
