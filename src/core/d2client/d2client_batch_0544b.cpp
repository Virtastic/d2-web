// src/core/d2client/d2client_batch_0544b.cpp -- two UNMAPPED 1.13c D2Client.dll rows reimplemented from
// the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane). Base 0x6FAB0000; every
// RVA below is retail's, and every global this file owns is a static named at its retail RVA.
//
// Both are MSVC 2003 Release CRT glue: __RTC_Initialize (0x0544B) and __lock_file
// (0x05590). Their observable is host state, so the one callee left unmapped in this tree (0x4860
// __lock, mapped in d2client_batch_04860.cpp) arrives as a seam;
// EnterCriticalSection is the kernel op the 04860 body also calls directly.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>
#include <Windows.h>   // EnterCriticalSection: host (kernel) state, a no-op under wasm

// +0x0D7840  DAT_6fb87840: the module's run-time error-check initialiser table. Retail's walk bounds
// are this same address on both sides, i.e. the shipped image registers no initialisers, so the loop
// makes no calls. The slot exists so the walk is written exactly as retail's.
static void (*g_rtcInitTable[1])(void);

// 1.13c D2Client+0x0544b (__RTC_Initialize, 61 B): walk the RTC initialiser table, invoking every
// non-NULL entry. The table is empty in this image, so the walk returns immediately.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_544b(void) {
    for (void (**p)(void) = g_rtcInitTable; p < g_rtcInitTable; ++p)
        if (*p != nullptr)
            (*p)();
}

// +0x0DB1F8  _iob[0]: the module's stream table, 19 records of 0x20 bytes (retail's range test
// +0x0DB1F7 < pf < +0x0DB459 covers records 0..18; _iob[1]/_iob[2] land on +0x0DB218/+0x0DB238).
#pragma pack(push, 4)
struct D2cFile {
    uint32_t ptr;       // 0x00
    int32_t  cnt;       // 0x04
    uint32_t base;      // 0x08
    int32_t  flag;      // 0x0C
    int32_t  file;      // 0x10
    int32_t  charbuf;   // 0x14
    int32_t  bufsiz;    // 0x18
    uint32_t tmpfname;  // 0x1C
};
#pragma pack(pop)
static_assert(sizeof(D2cFile) == 0x20, "FILE record stride");
static_assert(offsetof(D2cFile, base) == 0x08, "FILE._base");
static D2cFile g_iob[0x13];

// Callees this body reaches through the retail env (address = retail RVA).
struct D2ClientBatch0544BEnv {
    void (*lock)(int32_t locknum);   // 0x4860 __lock
};

// 1.13c D2Client+0x05590 (__lock_file, 47 B): take the stream's CRT lock. A stream inside the module's
// _iob table locks by index (lock 0x10 + stream index); any other stream enters the CRITICAL_SECTION
// that sits one FILE record past it.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_5590(D2cFile* file, const D2ClientBatch0544BEnv* env) {
    if (file >= &g_iob[0] && file < &g_iob[0x13]) {
        env->lock(0x10 + (int32_t)(file - &g_iob[0]));
        return;
    }
    EnterCriticalSection((LPCRITICAL_SECTION)(file + 1));
}

extern "C" void d2_client_47ab(uint32_t idx);   // 0x47AB _unlock

// 1.13c D2Client+0x055E2 (__unlock_file, 47 B): mirror of __lock_file -- an _iob stream releases lock
// 0x10 + index, any other stream leaves the CRITICAL_SECTION one FILE record past it.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_55e2(D2cFile* file) {
    if (file >= &g_iob[0] && file < &g_iob[0x13]) {
        d2_client_47ab(0x10 + (uint32_t)(file - &g_iob[0]));
        return;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)(file + 1));
}
