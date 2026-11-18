// src/core/gfx/fog_batch_1a52.cpp -- twelve UNMAPPED 1.13c Fog.dll rows reimplemented from the cached
// Ghidra decompiles (build-wasm/ghidra-fog/decomp/0x<RVA>.c) and cross-checked against the retail image
// retail/D2/D2SE/CORES/1.13c/Fog.dll (base 0x6FF50000). Alpha lane: no spec, no vector.
//
// All twelve are MSVC 2003 (VC7.1) Release CRT glue the linker pulled into Fog.dll: the lock 8 wrappers,
// __initterm/__cinit, doexit and the _exit/__exit/__cexit entrances, __time32, _vsprintf and _memmove.
// Prior art: src/core/gfx/d2ddraw_batch_rowex.cpp models the same CRT rows in D2DDraw.dll (doexit, the
// __initterm walker, __cinit's tables, the ExitProcess seam) and the one-heap substitution; the Fog
// globals below are read off Fog's own decompiles. This batch sits directly after Fog/batch2
// (src/core/gfx/fog_batch_1400.cpp, rows 0x1400..0x1A22).
//
// ponytail: wasm is single-threaded and has no Win32 critical sections or ExitProcess, so the lock table
// is a depth counter and the terminating call is a nullable host seam. Onexit rows 0x2445, 0x24C5, 0x24ED
// and 0x2525 are UNMAPPED outside this batch; the file-local copies below keep __cinit/doexit faithful
// without claiming those rows.
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <emscripten.h>

namespace {

typedef void (*FogExitFn)(void);
typedef int (*FogInitFn)(void);

// Fog+0x2E210: lock table, two dwords per lock. Retail's [slot*2] is a CRITICAL_SECTION*; the wasm build
// keeps the acquisition depth there so a nested lock/unlock pair is observable.
static uint32_t g_lock_table[0x40];
static void fog_lock(int idx) { ++g_lock_table[static_cast<size_t>(idx) * 2]; }
static void fog_unlock(int idx) {
    uint32_t* d = &g_lock_table[static_cast<size_t>(idx) * 2];
    if (*d != 0) --*d;
}

// Fog+0x2E018 (__xi, 6 dwords to +0x2E030) and Fog+0x2E000 (__xc, 5 dwords to +0x2E014). Slot [1] of
// __xi is retail 0x24C5 (___onexitinit) and is wired to the local copy; the remaining image entries
// (0x6791, 0x7C9C, 0x9705, 0x2473A and 0xB035, 0x24B40, 0x24B60, 0x24B80) point at rows not ported.
static FogInitFn g_xi[6];
static FogInitFn g_xc[5];
// Fog+0x2EACC: PTR___fpmath, zero in the shipped image.
static int (*g_fpmath)(int);

// Fog+0x5A8298 / Fog+0x5A8294: atexit table begin and next free slot (rows 0x2445/0x24ED/0x2525).
static FogExitFn* g_onexit_begin;
static FogExitFn* g_onexit_end;

// Fog+0x2E034..0x2E03C and Fog+0x2E040..0x2E048, the two exit-table ranges doexit walks. All four slots
// are zero except +0x2E044 = retail 0x2474D, a row not ported yet.
static FogExitFn g_exit_a[2];
static FogExitFn g_exit_b[2];

// Fog+0x30240 / +0x30244 / +0x3023C.
static uint32_t g_C_Termination_Done;
static uint32_t g_C_Exit_Done;
static uint8_t g_exitflag;

// Fog+0x1A22 ___crtExitProcess (implemented in batch2) is the terminating call on the non-returning
// doexit path; wasm has no ExitProcess, so it is a host seam and unset means "consume the code".
static void (*g_exit_process)(int);

static void fog_crt_exit_process(int code) {
    if (g_exit_process != nullptr) g_exit_process(code);
}

// Retail __initterm contract: walk [begin, end) ascending, call every non-NULL slot.
static void fog_initterm_range(FogExitFn* begin, FogExitFn* end) {
    for (FogExitFn* p = begin; p < end; ++p)
        if (*p != nullptr) (*p)();
}

// Fog+0x24C5 ___onexitinit: table = malloc(0x80), slot 0 NULL, end = begin, 0 on success; 0x18 (the
// CRT "not enough memory" code) when the allocation fails.
static int fog_onexit_init(void) {
    FogExitFn* t = static_cast<FogExitFn*>(std::malloc(0x80));
    if (t == nullptr) return 0x18;
    t[0] = nullptr;
    g_onexit_begin = t;
    g_onexit_end = t;
    return 0;
}

// Fog+0x2445/0x24ED/0x2525 __onexit_lk chain: append func, growing the table; 0 on success, -1 when the
// reallocation fails. (Retail sizes the block with __msize; the port has one heap, so this port's
// std::realloc grows from the exact element count.)
static int fog_atexit(FogExitFn func) {
    size_t used = static_cast<size_t>(g_onexit_end - g_onexit_begin);
    FogExitFn* p = static_cast<FogExitFn*>(std::realloc(g_onexit_begin, (used + 1) * sizeof(FogExitFn)));
    if (p == nullptr) return -1;
    g_onexit_begin = p;
    g_onexit_end = p + used;
    *g_onexit_end++ = func;
    return 0;
}

// Fog+0x3D69, the handler __cinit registers with _atexit: a __initterm walk over the empty table
// 0x6FF7CB50..0x6FF7CB50, so it makes no calls.
static void fog_atexit_walker(void) {}

static void fog_tables_init(void) {
    static bool done;
    if (done) return;
    done = true;
    g_xi[1] = fog_onexit_init;
}

// Fog+0x1AE6 doexit body, shared by _exit/__exit/__cexit.
static void fog_doexit(uint32_t code, int quick, int retcaller) {
    fog_lock(8);
    if (g_C_Exit_Done == 1) {                       // retail: TerminateProcess(GetCurrentProcess(), code)
        fog_crt_exit_process(static_cast<int>(code));
        return;
    }
    g_C_Termination_Done = 1;
    g_exitflag = static_cast<uint8_t>(retcaller);
    if (quick == 0) {
        if (g_onexit_begin != nullptr) {
            while (g_onexit_end != g_onexit_begin) {
                --g_onexit_end;
                if (*g_onexit_end != nullptr) (*g_onexit_end)();
            }
        }
        fog_initterm_range(g_exit_a, g_exit_a + 2);
    }
    fog_initterm_range(g_exit_b, g_exit_b + 2);
    if (retcaller != 0) fog_unlock(8);   // Fog+0x1B95: drop lock 8 unless param_3 == EDI (0)
    if (retcaller != 0) return;
    g_C_Exit_Done = 1;
    fog_crt_exit_process(static_cast<int>(code));
}

inline FogExitFn* fog_ptr(uint32_t v) {
    return reinterpret_cast<FogExitFn*>(static_cast<uintptr_t>(v));
}

}  // namespace

// Probe record ABI: arguments in retail register/stack order starting at r[0], result in r[4].
extern "C" {

// 1.13c Fog+0x01A52
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1a52(uint32_t* r) {
    (void)r;
    fog_lock(8);
}

// 1.13c Fog+0x01A5B
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1a5b(uint32_t* r) {
    (void)r;
    fog_unlock(8);
}

// 1.13c Fog+0x01A64
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1a64(uint32_t* r) {
    fog_initterm_range(fog_ptr(r[0]), fog_ptr(r[1]));
}

// 1.13c Fog+0x01A7C
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1a7c(uint32_t* r) {
    fog_tables_init();
    if (g_fpmath != nullptr) g_fpmath(static_cast<int>(r[0]));
    int result = 0;
    for (FogInitFn* p = g_xi; p < g_xi + 6; ++p) {
        if (result != 0) { r[4] = static_cast<uint32_t>(result); return; }
        if (*p != nullptr) result = (*p)();
    }
    if (result == 0) {
        fog_atexit(fog_atexit_walker);
        for (FogInitFn* p = g_xc; p < g_xc + 5; ++p)
            if (*p != nullptr) (*p)();
        result = 0;
    }
    r[4] = static_cast<uint32_t>(result);
}

// 1.13c Fog+0x01AE6
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1ae6(uint32_t* r) {
    fog_doexit(r[0], static_cast<int>(r[1]), static_cast<int>(r[2]));
}

// 1.13c Fog+0x01B95
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1b95(uint32_t* r) {
    if (r[0] != r[1]) fog_unlock(8);
}

// 1.13c Fog+0x01BA9
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1ba9(uint32_t* r) {
    fog_doexit(r[0], 0, 0);
}

// 1.13c Fog+0x01BBA
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1bba(uint32_t* r) {
    fog_doexit(r[0], 1, 0);
}

// 1.13c Fog+0x01BCB
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1bcb(uint32_t* r) {
    (void)r;
    fog_doexit(0, 0, 1);
}

// 1.13c Fog+0x01BDA
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1bda(uint32_t* r) {
    // GetSystemTimeAsFileTime -> 100 ns since 1601, minus 116444736000000000, divided by 10000000.
    const int32_t t = static_cast<int32_t>(std::time(nullptr));
    if (r[0] != 0) *reinterpret_cast<int32_t*>(static_cast<uintptr_t>(r[0])) = t;
    r[4] = static_cast<uint32_t>(t);
}

// 1.13c Fog+0x01C13
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1c13(uint32_t* r) {
    // wasm32's va_list is a 4-byte pointer held in the register; copy at most the two argument slots.
    va_list ap;
    std::memcpy(&ap, &r[2], sizeof(ap) <= 2 * sizeof(uint32_t) ? sizeof(ap) : 2 * sizeof(uint32_t));
    char* dest = reinterpret_cast<char*>(static_cast<uintptr_t>(r[0]));
    const char* fmt = reinterpret_cast<const char*>(static_cast<uintptr_t>(r[1]));
    // Retail's FILE is {_cnt = 0x7FFFFFFF, _flag = 0x42, _base = _ptr = dest}; the host formatter stands
    // in for the format engine at Fog+0x411F and terminates within _cnt bytes.
    const int n = dest != nullptr ? std::vsnprintf(dest, static_cast<size_t>(0x7FFFFFFF), fmt, ap) : 0;
    r[4] = static_cast<uint32_t>(n);
}

// 1.13c Fog+0x01C70
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1c70(uint32_t* r) {
    void* dst = reinterpret_cast<void*>(static_cast<uintptr_t>(r[0]));
    void* src = reinterpret_cast<void*>(static_cast<uintptr_t>(r[1]));
    r[4] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(std::memmove(dst, src, r[2])));
}

}  // extern "C"
