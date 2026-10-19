#include <cstdint>
#include <cstdlib>
#include <cstddef>

// 1.13c D2Gfx+0x1448 -- MSVC 2003 Release CRT _doexit, the module's process-exit path (retail base
// 0x6FA80000). Contract: __lock(8); if _C_Exit_Done (+0x10DE4) is already 1,
// TerminateProcess(GetCurrentProcess(), code) and never return; _C_Termination_Done (+0x10DE0) = 1;
// _exitflag (+0x10DDC) = (byte)retcaller; when quick == 0 run the onexit list backwards then
// __initterm(__xc_a, __xc_z); always __initterm(__xt_a, __xt_z); __unlock(8); if retcaller != 0 return;
// _C_Exit_Done = 1; ___crtExitProcess(code). Both __initterm tables hold a live entry
//, so the two walks
// below are real: __xc[1] = D2Gfx RVA 0x4094 (a C-initialiser wrapper) and __xt[1] = RVA 0xC95D (an
// import-forwarding thunk).
//
// Our engine has no process to terminate and never runs retail D2Gfx's CRT shutdown, so this is a shim:
// it models the two flag writes, the retcaller branch and the two __initterm walks, not TerminateProcess
// / ___crtExitProcess (no wasm analogue) nor the onexit list. The initializer and registration now
// share a real table below; running its callbacks on shutdown remains unimplemented.
static uint32_t g_C_Termination_Done;   // +0x10DE0
static uint32_t g_C_Exit_Done;          // +0x10DE4
static uint8_t  g_exitflag;             // +0x10DDC

// The CRT's __xc (initialiser) and __xt (terminator) tables at D2Gfx +0x10028 / +0x10034: two dwords
// each, a leading NULL slot then the entry slot. Retail's entries are D2Gfx RVA 0x4094 (__xc[1]: `call
// 0x6014`, then tail-jump 0x5E19 when the _exitflag byte at +0x10DDC is set) and RVA 0xC95D (__xt[1]:
// `push [0x6FA9D688]; call [IAT 0x6FA8D104]`). Neither is ported, so our entry slots stay 0 and the walks
// have nothing to call -- the retail slot shape, not the retail contents.
static uint32_t g_xc[2];                // +0x10028 (a), +0x10030 (z)
static uint32_t g_xt[2];                // +0x10034 (a), +0x1003C (z)

extern "C" void d2gfx_initterm(const uint32_t* begin, const uint32_t* end);

extern "C" void d2gfx_doexit(uint32_t code, uint32_t quick, uint32_t retcaller) {
    if (g_C_Exit_Done == 1) return;     // retail: TerminateProcess(GetCurrentProcess(), code)
    g_C_Termination_Done = 1;
    g_exitflag = (uint8_t)retcaller;    // byte store
    if (quick == 0) d2gfx_initterm(g_xc, g_xc + 2);   // retail step 5; onexit callback walk remains unimplemented
    d2gfx_initterm(g_xt, g_xt + 2);     // retail step 6: always
    (void)code;                         // code reaches only the terminating (unmodelled) paths
    if (retcaller != 0) return;         // ___crtExitProcess not reached
    g_C_Exit_Done = 1;
}

// The probes read the shim's CRT state back out (the vector records these globals).
extern "C" void d2gfx_doexit_snapshot(uint32_t* s) {
    s[0] = g_C_Termination_Done; s[1] = g_C_Exit_Done; s[2] = g_exitflag;
}

// 1.13c D2Gfx+0x13C6 -- MSVC 2003 Release CRT __initterm. Contract: walk
// [begin, end) in ascending order and call every non-NULL dword with no arguments; void return. Retail
// takes begin in EAX and end as its one cdecl stack argument -- a caller-side convention, so the shim takes
// the two pointers.
extern "C" void d2gfx_initterm(const uint32_t* begin, const uint32_t* end) {
    for (const uint32_t* p = begin; p < end; ++p) {
        uint32_t fn = *p;
        if (fn != 0) ((void (*)(void))(uintptr_t)fn)();
    }
}

// 1.13c D2Gfx+0x163C -- MSVC 2003 Release CRT _onexit. Contract: _lock(8)
// 0x13B4; retval = __onexit_lk(func) 0x1594; _unlock(8) 0x166E; return retval. __onexit_lk appends func
// to the atexit table ___onexitinit 0x1614 (malloc(0x80) at load, 32 slots, grown by realloc) and returns
// func; it returns 0 -- the failed realloc's eax -- only when the table cannot grow. There is no NULL
// check on func, so func == 0 is stored and 0 comes back.
// Retail callback slots remain 32-bit module addresses even on wasm64.
static uint32_t* g_atexit_begin;  // +0x1EA30
static uint32_t* g_atexit_end;    // +0x1EA2C
static size_t g_atexit_bytes;
static void* (*g_atexit_allocate)(size_t) = std::malloc; // retail malloc +0x28B3

// 1.13c D2Gfx+0x01614 -- ___onexitinit. Only the first slot is initialized.
// Allocation failure replaces begin with null but leaves the previous end untouched.
extern "C" int32_t d2gfx_onexitinit() {
    g_atexit_begin = static_cast<uint32_t*>(g_atexit_allocate(0x80));
    if (g_atexit_begin == nullptr) return 0x18;
    *g_atexit_begin = 0;
    g_atexit_end = g_atexit_begin;
    g_atexit_bytes = 0x80;
    return 0;
}

extern "C" uint32_t d2gfx_onexit(uint32_t func) {
    if (g_atexit_begin == nullptr) return 0;
    const size_t used = static_cast<size_t>(g_atexit_end - g_atexit_begin);
    if (g_atexit_bytes < used * 4 + 4) {
        const size_t grow = g_atexit_bytes < 0x800 ? g_atexit_bytes : 0x800;
        size_t bytes = g_atexit_bytes + grow;
        auto* table = static_cast<uint32_t*>(std::realloc(g_atexit_begin, bytes));
        if (table == nullptr) {
            bytes = g_atexit_bytes + 0x10;
            table = static_cast<uint32_t*>(std::realloc(g_atexit_begin, bytes));
            if (table == nullptr) return 0;
        }
        g_atexit_begin = table;
        g_atexit_end = table + used;
        g_atexit_bytes = bytes;
    }
    *g_atexit_end++ = func;
    return func;
}

// 1.13c D2Gfx+0x601D -- MSVC 2003 Release CRT __fptrap. The whole body is
// `push _RT_FLOAT (2); call __amsg_exit 0x18EB; pop ecx; ret`: cdecl(void), no arguments, no memory of
// its own touched, and the trailing `ret` is unreachable because __amsg_exit ends the process. It is the
// CRT's "floating point not loaded" hook: the three late-bound float slots _output 0x1E28 calls through
// (0x6FA906C0/C4/CC, _fltcvt/_cropzeros/_forcdecpt) are statically this address, so in 1.13c D2Gfx a
// %e/%f/%g directive is a CRT fatal error, not an output difference.
// Our engine has no process to end, no __amsg_exit and no float-through-a-CRT-slot path, so this is a
// shim for the one thing the function's bytes say and the only thing observable without ending the
// process: the callee and its argument word. It reports that pair as an event the probe renders into the
// hooklog, and returns 0 for the EAX retail's unreachable `ret` would have carried.
extern "C" uint32_t d2gfx_fptrap(uint32_t* ev) {
    ev[0] = 0x18EB;     // __amsg_exit, the function's only callee
    ev[1] = 2;          // _RT_FLOAT, its only argument word
    return 0;
}
