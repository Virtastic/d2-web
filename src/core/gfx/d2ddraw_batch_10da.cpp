// src/core/gfx/d2ddraw_batch_10da.cpp -- eleven UNMAPPED 1.13c D2DDraw.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2ddraw/decomp/0x<RVA>.c (alpha lane, no spec/vector yet). Retail base
// 0x6F8C0000; every RVA below is retail's and every module global is named at its retail RVA.
//
// The rows are MSVC 2003 (VC7.1) Release CRT glue the linker pulled into the DLL:
// the __lock/__unlock leaves around the per-thread data, the RTC initialiser walk, the TLS slot calls
// and the _ptiddata allocator/freer. They share the module's lock table at +0xE038 with the __onexit
// family in d2ddraw_batch_rowex.cpp, so those two helpers have external linkage here.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

// Shared with d2ddraw_batch_rowex.cpp: retail's lock table (+0xE038) and __amsg_exit (0x150F).
void d2ddraw_lock(int idx);
void d2ddraw_unlock(int idx);
extern "C" void d2ddraw_amsg_exit(int code);

// 1.13c D2DDraw+0x010DA: tail fragment into RVA 0x112B, which is __unlock(8). A second copy of the same
// leaf (the compiler emitted it once per template instantiation site); no arguments, returns nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_10da(void) { d2ddraw_unlock(8); }

// 1.13c D2DDraw+0x01AC3: tail fragment into 0x1A17 with lock number 10 -- __unlock(10).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_1ac3(void) { d2ddraw_unlock(10); }

// 1.13c D2DDraw+0x01ACC (__lock): if table[file**2] is NULL, __mtinitlocknum(file) creates the 0x18-byte
// critical-section record (modelled inside d2ddraw_lock as malloc + InitializeCriticalSection); a zero
// return runs __amsg_exit(0x11). Then EnterCriticalSection on the slot. Returns nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_1acc(int file) { d2ddraw_lock(file); }

// 1.13c D2DDraw+0x01AFD (__RTC_Initialize): walk the initialiser table at +0xCF38 and call every non-NULL
// entry. In the shipped image the walk's begin and end are the same address, so it makes no calls; the
// bounds are modelled as one empty table rather than collapsed, so the rule stays visible.
static void (*g_rtc_table[1])(void);   // +0xCF38, both bounds
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_1afd(void) {
    void (**begin)(void) = g_rtc_table;
    void (**end)(void) = g_rtc_table;
    for (void (**p)(void) = begin; p < end; ++p)
        if (*p != nullptr) (*p)();
}

// 1.13c D2DDraw+0x01B85: TlsAlloc(), result discarded. wasm has no TLS indices, so the allocator's
// failure value (TLS_OUT_OF_INDEXES) is returned -- the caller at 0x1DA5 ignores it either way.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2ddraw_1b85(void) { return 0xFFFFFFFFu; }

// __mtterm state: +0xE158 the TLS index (0xFFFFFFFF = TLS_OUT_OF_INDEXES, not allocated) and +0xF09C the
// TlsFree import pointer. __mtdeletelocks (RVA 0x19C2) is a row not ported yet, so it is a host seam.
static uint32_t g_tls_index = 0xFFFFFFFFu;   // +0xE158  DAT_6f8ce158
static void (*g_tls_free)(uint32_t);         // +0xF09C  DAT_6f8cf09c = TlsFree
static void (*g_mtdeletelocks)(void);        // RVA 0x19C2, a row not ported

// 1.13c D2DDraw+0x01B8E (__mtterm): if the TLS index is allocated, free it (TlsFree) and reset the slot
// to -1; then __mtdeletelocks(). Returns nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_1b8e(void) {
    if (g_tls_index != 0xFFFFFFFFu) {
        if (g_tls_free) g_tls_free(g_tls_index);
        g_tls_index = 0xFFFFFFFFu;
    }
    if (g_mtdeletelocks) g_mtdeletelocks();
}

// 1.13c D2DDraw+0x01BAB (__initptd): seed a thread-data block -- _initaddr +0x54 = the retail VA
// 0x6F8CE190 (stored as its RVA 0xE190, the port's convention for retail VAs) and _holdrand +0x14 = 1.
// The locale argument (_Locale) is not touched. Returns nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_1bab(uint8_t* ptd) {
    *(uint32_t*)(ptd + 0x54) = 0xE190u;   // _initaddr, retail 0x6F8CE190
    *(uint32_t*)(ptd + 0x14) = 1;         // _holdrand
}

// 1.13c D2DDraw+0x01BBE (__getptd): the current thread's _ptiddata, created on first use. Retail saves
// GetLastError, reads the TLS slot +0xE158, and on a miss callocs 0x8C bytes, stores the pointer back,
// seeds _initaddr +0x54 = 0x6F8CE190, _holdrand +0x14 = 1, _thandle +0x04 = -1 and _tid +0x00 =
// GetCurrentThreadId(); a failed allocation or store runs __amsg_exit(0x10); the saved error is restored
// before returning. The engine is single-threaded, so the TLS slot is this one static pointer and _tid is
// the engine's only thread (1); wasm has no last-error word to save.
static uint8_t* g_ptd_tls;   // +0xE158 TLS slot (single-threaded)
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2ddraw_1bbe(void) {
    uint8_t* ptd = g_ptd_tls;
    if (ptd == nullptr) {
        ptd = (uint8_t*)std::calloc(1, 0x8C);
        if (ptd == nullptr) { d2ddraw_amsg_exit(0x10); return nullptr; }
        g_ptd_tls = ptd;                    // TlsSetValue(index, ptd) != 0
        d2_fn_d2ddraw_1bab(ptd);
        *(uint32_t*)(ptd + 0x04) = 0xFFFFFFFFu;   // _thandle
        *(uint32_t*)(ptd + 0x00) = 1;             // _tid
    }
    return ptd;
}

// Rows referenced by __freefls below.
extern "C" void d2_fn_d2ddraw_1d61(void);
extern "C" void d2_fn_d2ddraw_1d6d(void);

// Statics the __freefls comparisons name: +0x10288 a tlocinfo object, +0xE430 another static tlocinfo,
// and +0xE484 (PTR_DAT) the image's default-locale pointer. ___freetlocinfo (RVA 0x3CBB) is not ported,
// so it is a host seam.
static uint8_t g_tlocinfo_10288;             // DAT_6f8d0288
static uint8_t g_tlocinfo_static_e430;       // DAT_6f8ce430
static int*    g_tlocinfo_default;           // PTR_DAT_6f8ce484
static void (*g_freetlocinfo)(void*);

// 1.13c D2DDraw+0x01C2F (__freefls@4): free a thread-data block. NULL is ignored. Each of the six owned
// pointers at +0x24, +0x2C, +0x34, +0x3C, +0x44, +0x48 is freed when non-NULL; the _initaddr slot at
// +0x54 is freed unless it still holds the static marker 0xE190. Then, under lock 0xD, the refcounted
// object at +0x60 is decremented and freed at zero unless it is the +0x10288 singleton; unlock 0xD. Under
// lock 0xC, the tlocinfo at +0x64 is decremented -- its own count plus the counts of the five sub-objects
// it owns (slots 0xB, 0xD, 0xC, 0x10 and the object behind slot 0x13 at +0xB4) -- and freed at zero
// unless it is the default (+0xE484) or the static +0xE430 record; unlock 0xC. Finally the block itself
// is freed. Returns nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_1c2f(void* ptd) {
    if (ptd == nullptr) return;
    uint8_t* p = (uint8_t*)ptd;
    static const uint32_t kOwned[] = {0x24, 0x2C, 0x34, 0x3C, 0x44, 0x48};
    for (uint32_t off : kOwned)
        if (*(void**)(p + off) != nullptr) std::free(*(void**)(p + off));
    if (*(uint32_t*)(p + 0x54) != 0xE190u) std::free(*(void**)(p + 0x54));

    d2ddraw_lock(0xD);
    int* r = *(int**)(p + 0x60);
    if (r != nullptr) {
        --*r;
        if (*r == 0 && r != (int*)(uintptr_t)&g_tlocinfo_10288) std::free(r);
    }
    d2_fn_d2ddraw_1d61();                 // unlock(0xD)

    d2ddraw_lock(0xC);
    int* q = *(int**)(p + 0x64);
    if (q != nullptr) {
        --*q;
        if (q[0x0B] != 0) --*(int*)(uintptr_t)(uint32_t)q[0x0B];
        if (q[0x0D] != 0) --*(int*)(uintptr_t)(uint32_t)q[0x0D];
        if (q[0x0C] != 0) --*(int*)(uintptr_t)(uint32_t)q[0x0C];
        if (q[0x10] != 0) --*(int*)(uintptr_t)(uint32_t)q[0x10];
        --*(int*)(uintptr_t)((uint32_t)q[0x13] + 0xB4);
        if (q != g_tlocinfo_default && q != (int*)(uintptr_t)&g_tlocinfo_static_e430 && *q == 0)
            if (g_freetlocinfo) g_freetlocinfo(q);
    }
    d2_fn_d2ddraw_1d6d();                 // unlock(0xC)

    std::free(ptd);
}

// 1.13c D2DDraw+0x01D61: tail fragment into 0x1A17 with lock number 0xD -- __unlock(0xD).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_1d61(void) { d2ddraw_unlock(0xD); }

// 1.13c D2DDraw+0x01D6D: tail fragment into 0x1A17 with lock number 0xC -- __unlock(0xC).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_1d6d(void) { d2ddraw_unlock(0xC); }

// Host-side installation of the unported seams (0x19C2 __mtdeletelocks, TlsFree, 0x3CBB
// ___freetlocinfo) and the default-locale pointer, for a later function probe or bring-up.
extern "C" void d2ddraw_crt_thread_seams(void (*mtdeletelocks)(void), void (*tls_free)(uint32_t),
                                         void (*freetlocinfo)(void*), int* default_tlocinfo) {
    g_mtdeletelocks = mtdeletelocks;
    g_tls_free = tls_free;
    g_freetlocinfo = freetlocinfo;
    g_tlocinfo_default = default_tlocinfo;
}
