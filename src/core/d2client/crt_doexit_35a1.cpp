// D2Client+0x35A1 = MSVC 2003 Release CRT _doexit(code, quick, returning): the shared termination
// path. Every external effect is a callee, so the rule is expressed against the host seam in retail's
// call order (the crt_chsize_bb34.cpp pattern); the module state retail reads and writes is the
// statics below, each named with its retail RVA.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"

namespace {
constexpr uint32_t kSehProlog = 0x4904;        // injected __SEH_prolog, scope table 0x6FB7F180
constexpr uint32_t kLock = 0x4860;             // __lock
constexpr uint32_t kUnlock = 0x47AB;           // _unlock
constexpr uint32_t kInitTerm = 0x351F;         // __initterm: begin in EAX, end on the stack
constexpr uint32_t kCrtExitProcess = 0x34DD;   // ___crtExitProcess
// The two imports are called through their IAT slots: 0x6FB7EF68 / 0x6FB7EF44 minus image base.
constexpr uint32_t kGetCurrentProcess = 0xCEF68, kTerminateProcess = 0xCEF44;
static_assert(kGetCurrentProcess + 0x6FAB0000u == 0x6FB7EF68u, "GetCurrentProcess IAT slot");
static_assert(kTerminateProcess + 0x6FAB0000u == 0x6FB7EF44u, "TerminateProcess IAT slot");
constexpr uint32_t kScopeTable = 0xCF180;      // 0x6FB7F180: pushed by the SEH prolog
// __initterm table pairs: C initialisers then C++ initialisers.
constexpr uint32_t kXcBegin = 0xDB07C, kXcEnd = 0xDB084;   // 0x6FB8B07C .. 0x6FB8B084
constexpr uint32_t kXiBegin = 0xDB088, kXiEnd = 0xDB090;   // 0x6FB8B088 .. 0x6FB8B090

// Module state, all zero-initialised.
static uint32_t g_onexitEnd;    // +0x124AD0  0x6FBD4AD0: next slot down the onexit table
static uint32_t g_onexitBegin;  // +0x124AD4  0x6FBD4AD4: table base, NULL until the table exists
static uint32_t g_exitStarted;  // +0x0FA8E0  0x6FBAA8E0
static uint8_t  g_returning;    // +0x0FA8DC  0x6FBAA8DC: (byte)param_3
static uint32_t g_exiting;      // +0x0FA8E4  0x6FBAA8E4: 1 once the process is being terminated

inline uint32_t go(D2cHost h, uint32_t rva, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3) {
    D2cCall c = {};
    c.rva = rva;
    c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3;
    return h(&c);
}

// __initterm / the onexit entries take no stack arguments: the code address itself is the call.
inline uint32_t goEax(D2cHost h, uint32_t rva, uint32_t eax, uint32_t s0) {
    D2cCall c = {};
    c.rva = rva;
    c.eax = eax;
    c.s[0] = s0;
    return h(&c);
}
}  // namespace

// 1.13c D2Client+0x35A1. cdecl; [esp+4] = code, [esp+8] = quick, [esp+0xC] = returning. Void.
// The entry SEH prolog only builds a stack frame (0x4904 with the 0x6FB7F180 scope table and 8);
// it touches no module state, so it is stated here and not seamed.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_35a1(uint32_t code, uint32_t quick, uint32_t returning,
                                        D2cHost h) {
    go(h, kSehProlog, kScopeTable, 8, 0, 0);           // __SEH_prolog(&DAT_6fb7f180, 8)
    go(h, kLock, 8, 0, 0, 0);                          // __lock(8)
    if (g_exiting == 1) {                              // a nested exit terminates the process now
        uint32_t hProcess = go(h, kGetCurrentProcess, 0, 0, 0, 0);
        go(h, kTerminateProcess, hProcess, code, 0, 0);   // (hProcess, uExitCode); no return
    }
    g_exitStarted = 1;
    g_returning = (uint8_t)returning;
    if (quick == 0) {
        if (g_onexitBegin != 0) {
            // Unsigned `jb` on the raw address: one slot is 4 bytes, and a slot may be NULL.
            for (;;) {
                g_onexitEnd -= 4;
                if (g_onexitBegin > g_onexitEnd) break;
                uint32_t fn = *(const uint32_t*)(uintptr_t)g_onexitEnd;
                if (fn != 0) go(h, fn, 0, 0, 0, 0);
            }
        }
        goEax(h, kInitTerm, kXcBegin, kXcEnd);         // __initterm(__xc_a, __xc_z)
    }
    goEax(h, kInitTerm, kXiBegin, kXiEnd);             // __initterm(__xi_a, __xi_z)
    // 0x3650 is the injected __SEH_epilog fragment: EDI is 0 here, so it releases CRT lock 8 iff
    // the caller's [EBP+0x10] (returning) differs from 0.
    if (returning != 0) go(h, kUnlock, 8, 0, 0, 0);
    if (returning != 0) return;
    g_exiting = 1;
    go(h, kCrtExitProcess, code, 0, 0, 0);             // ___crtExitProcess(code); no return
}
