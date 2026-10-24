// src/core/d2client/d2client_batch_3664.cpp -- five 1.13c D2Client.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000; every RVA below is
// retail's, and every global this file owns is a static at its retail RVA comment.
//
// All five are MSVC 2003 Release CRT glue the linker pulled into the DLL:
// `_exit`, `__initterm`, an SEH fragment, `__SEH_epilog` and the lock-table `_unlock`. Their observable
// is a host operation (process exit, a table call, a kernel critical section), so each CRT call is a
// host seam, the src/core/d2client/d2client_batch_1e70.cpp pattern: the function takes a
// D2ClientBatch3664Env AFTER its retail arguments, which keep their order.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <emscripten.h>
#include <Windows.h>   // LeaveCriticalSection: host (kernel) state, a no-op under wasm

struct D2ClientBatch3664Env {
    void (*doexit)(uint32_t code, uint32_t quick, uint32_t returning_); // 0x35A1: runs the CRT exit paths
    void (*invoke)(uint32_t entry);                                     // one __initterm `call eax`
};

// Retail module state, all zero-initialised.
static uint8_t  g_csTable[0x200];   // +0x0DB0C8  DAT_6fb8b0c8: CRT lock table, 8-byte CRITICAL_SECTION stride
static uint32_t g_exceptionList;    // FS:[0] chain head (the SEH frame list; single-threaded here)

// LeaveCriticalSection(&DAT_6fb8b0c8[idx * 2]) -- the decompile indexes the table as dwords, so the
// byte stride is 8.
static void leave_cs(uint32_t idx) {
    LeaveCriticalSection((LPCRITICAL_SECTION)(g_csTable + (size_t)idx * 8));
}

// 1.13c D2Client+0x03664 (_exit): cdecl, `doexit(code, quick=0, returning=0)` -- the full termination
// path (CRT lock 8, onexit list, both initialiser tables, then process exit). A browser host cannot
// exit, so the seam stands in for doexit and the signed code is the only value that crosses.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_3664(int32_t code, const D2ClientBatch3664Env* env) {
    env->doexit((uint32_t)code, 0, 0);
}

// 1.13c D2Client+0x0351F (__initterm): private CRT convention -- begin arrives in EAX, end is the stack
// argument. Walk [begin, end) in ascending dword order; every non-NULL entry is called with no
// arguments. The bound is an unsigned `jb`, so end <= begin makes no calls (no do/while foot). Each
// entry is a retail initialiser address, hence the host seam for the call; the walk is the rule.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_351f(uint32_t* begin, uint32_t* end,
                                        const D2ClientBatch3664Env* env) {
    for (uint32_t* p = begin; p < end; ++p)
        if (*p != 0) env->invoke(*p);
}

// 1.13c D2Client+0x03650: SEH-injected fragment inside doexit (its operands are doexit's own frame and
// EDI, not an argument list): if the caller's [EBP+0x10] differs from EDI, call 0x47AB(8) -- release
// CRT lock 8. Both operands belong to the caller, so they are taken as arguments.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_3650(uint32_t stack10, uint32_t edi) {
    if (stack10 != edi) leave_cs(8);
}

// 1.13c D2Client+0x0493F (__SEH_epilog): injected epilogue -- ExceptionList = [EBP-4] (byte -0x10),
// then [EBP] is restored from the saved return address. The return-address half is the host ABI's own
// store; the engine-visible effect is the exception-list head, recorded here.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_493f(const uint32_t* ebp) {
    g_exceptionList = ebp[-4];
}

// 1.13c D2Client+0x047AB: the CRT `_unlock(idx)` -- LeaveCriticalSection(&DAT_6fb8b0c8[idx * 2]), the
// same table leave_cs addresses. The critical section is kernel handle state; the index is the rule.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_47ab(uint32_t idx) {
    leave_cs(idx);
}

// 1.13c D2Client+0x3664 (_exit), direct entry: doexit(code,0,0) then process exit; the process exit is the platform boundary.
extern "C" [[noreturn]] void D2Client_exit_3664(int32_t code) { std::_Exit(code); }
