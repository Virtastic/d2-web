#include "mcp_onexit.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

// 1.13c D2MCPClient+0x1187: the CRT's `_exit(code)` = doexit(code, quick=0,
// returning=0): run the registered onexit callbacks and the C tables, then terminate the process with `code`.
// It never returns and touches no game state, so the stand-in is process termination with the same code, as the
// project already does for retail's other fatal paths (src/core/d2common/exports.cpp).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_exit(int code) {
    std::exit(code);
}

// 1.13c D2MCPClient+0x10C4: the MSVC 2003 Release CRT `_doexit` the three
// wrappers above call. Retail (base 0x6FA20000): `__lock(8)`; if `_C_Exit_Done` (+0xAA24) is already 1,
// `TerminateProcess(GetCurrentProcess(), code)` and never return; `_C_Termination_Done` (+0xAA20) = 1;
// `_exitflag` (+0xAA1C) = (byte)retcaller; when `quick == 0` run the onexit list backwards (head +0x125D0,
// end pointer +0x125CC) and `__initterm` the C-initialiser table [0xA01C, 0xA020); always `__initterm` the
// C-terminator table [0xA024, 0xA028); the shared unlock helper 0x1173; if `retcaller != 0` return;
// `_C_Exit_Done` = 1; `___crtExitProcess(code)` (+0x1000, `ExitProcess`). Both `__initterm` tables are
// EMPTY in the shipped image -- one NULL slot each (read from retail 1.13c D2MCPClient.dll, SHA-256
// d40431969cab1cf9b7335dc3de4a51c1330527cc068d4caf9ce2dc1bf67469be) -- and the onexit head/tail are 0
// (unbacked .data), before initialization; registered native callbacks are shared with __onexit.
//
// The native callback table is shared with init/registration. The terminal host-process paths remain
// a shim; empty-table FN vectors still observe only these three globals. Native exhaustion leaves
// end == begin instead of retail's one-before-begin sentinel, avoiding invalid C++ pointer arithmetic.
// 1.13c D2Win+0x0113b: the same statically linked MSVC 2003 CRT `_doexit`, 181 B, identical
// control flow (globals 0x6F901020/24/1C, onexit 0x6F9ABC4C/50, tables 0x6F8FF034/3C); d2_mcp_doexit below is the stand-in.
static uint32_t g_C_Termination_Done;   // +0xAA20
static uint32_t g_C_Exit_Done;          // +0xAA24
static uint8_t  g_exitflag;             // +0xAA1C

extern "C" void d2_mcp_doexit(uint32_t code, uint32_t quick, uint32_t retcaller) {
    {
        d2_mcp_lock8_1030();
        struct Unlock { ~Unlock() { d2_mcp_unlock8_1039(); } } unlock;
        if (g_C_Exit_Done == 1) return;     // retail: TerminateProcess(GetCurrentProcess(), code)
        g_C_Termination_Done = 1;
        g_exitflag = (uint8_t)retcaller;    // byte store
        if (quick == 0 && g_mcp_onexit.onexit_begin != nullptr) {
            while (g_mcp_onexit.onexit_end > g_mcp_onexit.onexit_begin) {
                void* callback = *--g_mcp_onexit.onexit_end;
                if (callback != nullptr) reinterpret_cast<void (*)()>(callback)();
            }
        }
    } // Retail unlocks before the return/final-exit decision; also release on native unwinding.
    (void)code;                         // code reaches only the terminating (unmodelled) paths
    if (retcaller != 0) return;         // ___crtExitProcess not reached
    g_C_Exit_Done = 1;
}

// The probe reads the shim's CRT state back out (the vector records these globals).
extern "C" void d2_mcp_doexit_snapshot(uint32_t* s) {
    s[0] = g_C_Termination_Done; s[1] = g_C_Exit_Done; s[2] = g_exitflag;
}

// 1.13c D2MCPClient+0x011A9 (15 B, caller 0x011B8 __CRT_INIT@12): `__cexit` = doexit(0, quick=0,
// retcaller=1): run the onexit list and the C tables, then return instead of terminating. The onexit
// table uses the same native callbacks as __onexit; the shipped __initterm tables are empty.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_cexit_11a9(void) {
    d2_mcp_doexit(0, 0, 1);
}

// 1.13c D2MCPClient+0x5603 (size 322, caller 0x4794 report_failure): the MSVC 2003 Release CRT
// `__report_gsfailure`, the GS (buffer-security-cookie) failure report. Retail (base 0x6FA20000):
// `param_1 == 1` selects "Buffer overrun detected!", anything else "Unknown security failure detected!".
// If the security handler at +0xADAC (DAT_6fa2adac) is set it is called with no arguments and the
// message path is skipped; otherwise retail asks the host for the module path (GetModuleFileNameA into
// a 260-byte buffer, "<program name unknown>" if it fails), shortens it to a 60-byte display window
// with a "..." tail when strlen + 11 > 60, assembles the detected text, the module path and the long
// explanation with repeated "\n\n" separators (0x240A0/0x240B0), and shows that through
// 0x456D ___crtMessageBoxA with caption "Microsoft Visual C++ Runtime Library" and flags 0x12010.
// Either branch then calls 0x1198 __exit(3), which never returns; a `swi 3` follows if it did.
//
// ponytail: the dialog and module-path text are host state (no wasm analogue, not a protocol byte), so
// the shim keeps only the branch decision and the exit code -- add the message assembly if a vector
// ever needs the box text.
static void (*g_securityHandler)(void);   // +0xADAC, DAT_6fa2adac: optional GS handler

extern "C"
EMSCRIPTEN_KEEPALIVE int d2_mcp_report_gsfailure(uint32_t failureKind) {
    (void)failureKind;                    // selects the box text only (1 = overrun, else unknown)
    if (g_securityHandler != nullptr) g_securityHandler();
    return 3;                             // retail: __exit(3), never returns
}

// 1.13c D2MCPClient+0x41A0: the MSVC 2003 Release CRT `_strlen`, reached from the MCP client's own code
// (callers 0x1F46, 0x25EC, 0x5603, 0x5950). Byte scan until the pointer is dword-aligned -- each step reads
// the dword at that address but only its low byte decides -- then aligned dwords, stopping at the first that
// contains a zero byte. That test is the retail `(*p ^ 0xffffffff ^ *p + 0x7efefeff) & 0x81010100`; byte
// order then picks the offset 0..3. Returns the count before the NUL (0 for an empty string).
extern "C" EMSCRIPTEN_KEEPALIVE size_t d2_mcp_strlen(const char* s) {
    const char* p = s;
    while (((uintptr_t)p & 3) != 0) {
        if (*p == '\0') return (size_t)(p - s);   // retail loads a dword here but tests only the low byte
        ++p;
    }
    for (;;) {
        uint32_t v;
        memcpy(&v, p, 4);
        if (((v ^ 0xFFFFFFFFu ^ (v + 0x7EFEFEFFu)) & 0x81010100u) == 0) { p += 4; continue; }
        if ((v & 0x000000FFu) == 0) return (size_t)(p - s);
        if ((v & 0x0000FF00u) == 0) return (size_t)(p - s + 1);
        if ((v & 0x00FF0000u) == 0) return (size_t)(p - s + 2);
        return (size_t)(p - s + 3);
    }
}

// 1.13c D2MCPClient+0x0105A
// (size 106, caller 0x11B8 __CRT_INIT@12): the MSVC 2003 Release CRT `__cinit(param)`. Retail (base
// 0x6FA20000): if the C dynamic-initializer hook at +0x125D8 (DAT_6fa325d8, unbacked .data = NULL) is
// set, call it with `param`. Then walk the C-initializer table +0xA00C..+0xA018 -- 3 slots, read from
// the shipped image as [NULL, 0x1654 ___onexitinit, 0x34DE ___initmbctable] -- stopping at the first
// nonzero status. When every slot succeeded, register the exit thunk 0x170A with +0x16B4 _atexit,
// walk the C++-initializer table +0xA000..+0xA008 -- 2 slots, [NULL, 0x559D ___security_init_cookie] --
// calling every non-NULL slot and ignoring its status, then return 0.
//
// The tables have the retail slot counts. The 0x1654 entry is bound to the shared onexit initializer;
// 0x34DE (___initmbctable) remains a seam; 0x559D initializes the shared cookie. The 0x170A registration
// call uses the shared one-argument _atexit path and is consumed by doexit. See
// for the remaining boundary and capture requirements.
static int (*g_mcp_cinit_dyn_init)(int);    // +0x125D8 DAT_6fa325d8
extern "C" int32_t d2_mcp_onexitinit(void);
static int (*g_mcp_xi[3])(void) = {nullptr, d2_mcp_onexitinit, nullptr}; // +0xA00C: [NULL, ___onexitinit, ___initmbctable]
extern "C" void d2_mcp_security_init_cookie(void);
static void (*g_mcp_xc[2])(void) = {nullptr, d2_mcp_security_init_cookie};            // +0xA000: [NULL, ___security_init_cookie]

// The cinit table state is still a seam. Use a native function token for the retail code pointer:
// RVA 0x170A is not a valid wasm64 function pointer. The real one-argument _atexit path appends it to
// the same callback table that __onexitinit and doexit use.
static void mcp_cinit_exit_thunk_170a(void) {}
extern "C" int32_t d2_mcp_atexit_16b4(void* func);

extern "C"
EMSCRIPTEN_KEEPALIVE int d2_mcp_cinit(int param) {
    if (g_mcp_cinit_dyn_init != nullptr) g_mcp_cinit_dyn_init(param);
    int result = 0;
    for (size_t i = 0; i < 3; ++i) {
        if (result != 0) return result;     // retail checks before every slot, including the first
        if (g_mcp_xi[i] != nullptr) result = g_mcp_xi[i]();
    }
    if (result == 0) {
        // 1.13c registers LAB_6fa2170a through _atexit after all C initializers succeed and before
        // entering the C++ table. The one-argument callee appends to the shared initialized table.
        (void)d2_mcp_atexit_16b4(reinterpret_cast<void*>(&mcp_cinit_exit_thunk_170a));
        for (size_t i = 0; i < 2; ++i)
            if (g_mcp_xc[i] != nullptr) g_mcp_xc[i]();
        result = 0;
    }
    return result;
}

// 1.13c D2MCPClient+0x04670: the MSVC 2003 Release CRT `_strncpy`, reached from the MCP client's own code
// (callers 0x5603, 0x25EC -- the same two callers as `_strlen`). Copy bytes from `src` to `dst`; stop at the
// source NUL or after `count` bytes, then NUL-fill whatever remains of `count`. Retail walks unaligned bytes
// one at a time, then dword-aligns both sides and dword-copies while no dword contains a zero byte (same
// `(*p ^ 0xffffffff ^ *p + 0x7efefeff) & 0x81010100` test as `_strlen`), writing the partial dword truncated
// at the NUL. Those reads are an implementation detail: the bytes written are exactly the run of `src` up to
// its NUL, padded with zeros to `count`. Returns `dst`.
extern "C"
EMSCRIPTEN_KEEPALIVE char* d2_mcp_strncpy(char* dst, const char* src, size_t count) {
    char* d = dst;
    while (count != 0 && *src != '\0') { *d++ = *src++; --count; }
    while (count != 0) { *d++ = '\0'; --count; }
    return dst;
}
