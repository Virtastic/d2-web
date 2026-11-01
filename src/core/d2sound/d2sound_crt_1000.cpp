// src/core/d2sound/d2sound_crt_1000.cpp -- D2Sound.dll's statically linked MSVC 2003 CRT exit path and
// rand(), from the decompiles. Locks are no-ops (single wasm thread); the terminator tables are the
// retail ones, held as arrays the module's init fills. Not ported: __cinit (0x105A) and __CRT_INIT
// (0x11DA), which walk linker-placed init tables and the CRT heap/ioinfo/mtinit internals.
#include <cstdint>
#include <cstdlib>
#include <unistd.h>

typedef void (*d2s_fn)(void);

static d2s_fn* g_atexitBegin;          // 0x6F9C6990  atexit table base
static d2s_fn* g_atexitEnd;            // 0x6F9C698C  one past the last registered entry
static d2s_fn  g_preTerm[1];           // 0x6F9C5024..502C  C pre-terminators (empty here)
static d2s_fn  g_term[1];              // 0x6F9C502C..5034  C terminators (empty here)
static uint32_t g_exitFlag;            // 0x6F9C6040
static uint8_t  g_quickExit;           // 0x6F9C603C
static uint32_t g_exiting;             // 0x6F9C6044
static uint32_t g_holdrand = 1;        // per-thread _holdrand; one thread here

// 1.13c D2sound+0x1000  ___crtExitProcess: calls mscoree!CorExitProcess when loaded, then ExitProcess.
static void crtExitProcess(int code) { _exit(code); }

// 1.13c D2sound+0x1030  takes CRT lock 8 (__lock)
static void crt_lock8(void) {}

// 1.13c D2sound+0x1039  releases CRT lock 8 (__unlock)
static void crt_unlock8(void) {}

// 1.13c D2sound+0x1042  __initterm: call every non-null entry in [first, last)
static void initterm(d2s_fn* first, d2s_fn* last) {
    for (; first < last; ++first)
        if (*first) (*first)();
}

// 1.13c D2sound+0x1173  doexit's unlock tail: releases lock 8 unless the caller (param_3) kept it
static void doexit_unlock(int retainLock) { if (!retainLock) crt_unlock8(); }

// 1.13c D2sound+0x10C4  _doexit(code, quick, retain)
static void doexit(unsigned code, int quick, int retain) {
    crt_lock8();
    if (g_exiting == 1) _exit((int)code);          // re-entered during exit: TerminateProcess
    g_exitFlag = 1;
    g_quickExit = (uint8_t)retain;
    if (quick == 0) {
        if (g_atexitBegin) {
            while (--g_atexitEnd, g_atexitBegin <= g_atexitEnd)
                if (*g_atexitEnd) (*g_atexitEnd)();
        }
        initterm(g_preTerm, g_preTerm + 0);
    }
    initterm(g_term, g_term + 0);
    doexit_unlock(retain);
    if (retain) return;
    g_exiting = 1;
    crtExitProcess((int)code);
}

// 1.13c D2sound+0x1187  _exit
void d2sound_exit(int code) { doexit((unsigned)code, 0, 0); }

// 1.13c D2sound+0x1198  __exit
void d2sound__exit(unsigned code) { doexit(code, 1, 0); }

// 1.13c D2sound+0x11A9  __cexit
void d2sound_cexit(void) { doexit(0, 0, 1); }

// 1.13c D2sound+0x11B8  _rand: MSVC LCG on the per-thread holdrand
int d2sound_rand(void) {
    g_holdrand = g_holdrand * 0x343fdu + 0x269ec3u;
    return (int)((g_holdrand >> 16) & 0x7fff);
}
