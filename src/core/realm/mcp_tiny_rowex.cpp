// src/core/realm/mcp_tiny_rowex.cpp -- six UNMAPPED 1.13c D2MCPClient.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2mcpclient/decomp/0x<RVA>.c (alpha lane). Base 0x6FA20000; every RVA
// below is retail's, and every global this file owns is a static named at its retail RVA.
//
// All five are MSVC 2003 (VC7.1) Release CRT leaves the linker pulled into the module. Where the leaf
// reaches kernel/host state (the critical section) or module state no argument pins (the new handler, the
// cookie, the unwind record) that operand is a seam named in the comment; everything else is the rule.
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>
#include <Windows.h>   // LeaveCriticalSection: host (kernel) state, a no-op under the wasm compat shim

// +0x6FA2A038 DAT_6fa2a038: the CRT lock table, dword entries at an 8-byte stride; each entry is the
// critical-section pointer 0x14EE hands to LeaveCriticalSection. Zero-initialised.
static uint32_t g_lockTable[16];

// LeaveCriticalSection(_locktable[idx]) -- the same read 0x14EE (_unlock) makes.
static void leave_cs(uint32_t idx) {
    LeaveCriticalSection((LPCRITICAL_SECTION)(uintptr_t)g_lockTable[idx * 2]);
}

// 1.13c D2MCPClient+0x01C07: `push 4; call 0x14EE; pop ecx; ret` -- the 9-byte CRT wrapper releasing
// lock 4.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock4_1c07(void) {
    leave_cs(4);
}

// 1.13c D2MCPClient+0x01CEB: the identical 9-byte wrapper at 0x1CEB (`push 4; call 0x14EE; pop ecx;
// ret`), reached from the allocation helper 0x1C25 after its small-block heap call. Same rule.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock4_1ceb(void) {
    leave_cs(4);
}

// 1.13c D2MCPClient+0x028B3: the same 9-byte wrapper (`push 4; call 0x14EE; pop ecx; ret`) reached from
// the small-block allocator 0x2841; releasing lock 4 is the whole rule.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock4_28b3(void) {
    leave_cs(4);
}

// +0x6FA2A418 DAT_6fa2a418: the CRT's unwind-call record written by the SEH hook 0x29D2. Ghidra splits it
// into DAT_6fa2a41c (handler), DAT_6fa2a420 (handler argument) and DAT_6fa2a424 (frame EBP); dword 0 is
// never touched.
static uint32_t g_unwindCall[4];

// 1.13c D2MCPClient+0x029D2: the SEH-injected fragment __local_unwind2 (0x293E) runs before it calls a
// __finally handler. Its operands are the caller's, not an argument list -- EAX is the handler address,
// [EBP+8] is the 0x101 the caller pushed, EBP is the caller's frame -- so they arrive here as arguments
// (the 0x03650 pattern, src/core/d2client/d2client_batch_3664.cpp). Body: record [4]=handler,
// [8]=argument, [0xC]=frame; the `ret 4` only pops the caller's argument. No reader in the image.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_unwind_call_29d2(uint32_t handler, uint32_t arg, uint32_t frame) {
    g_unwindCall[1] = handler;
    g_unwindCall[2] = arg;
    g_unwindCall[3] = frame;
}

// 1.13c D2MCPClient+0x0401F (__callnewh): cdecl `int __cdecl __callnewh(size_t _Size)`. Load
// DAT_6fa2ad14, the CRT's installed new-handler (`_pnh`); when it is non-NULL call it with the size and
// answer 1 only if it returns non-zero, else 0. No recovered row writes _pnh, so its value is the seam,
// passed after the retail argument.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_callnewh(uint32_t size, int32_t (*handler)(uint32_t)) {
    if (handler == nullptr) return 0;
    return handler(size) != 0 ? 1 : 0;
}

// +0x6FA2A6C0 DAT_6fa2a6c0: __security_cookie (zero-initialised until the CRT security init sets it).
static uint32_t g_stackCookie;

// 1.13c D2MCPClient+0x047C5 (__security_check_cookie): fastcall, the expected cookie arrives in ECX.
// `cmp ecx, __security_cookie; jne report_failure; ret` -- equal returns; unequal tail-jumps to 0x4794
// report_failure, which never returns (ExitProcess(3) on its final path). The compare is the rule; the
// terminating branch is the engine's process-exit stand-in, src/core/realm/mcp_exit.cpp.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_security_check_cookie(uint32_t cookie) {
    if (cookie == g_stackCookie) return;
    std::exit(3);
}

// 1.13c D2MCPClient+0x01030: the 9-byte CRT wrapper `push 8; call 0x15A3 (__lock); pop ecx; ret`,
// reached from 0x0167C __onexit. Taking exit lock 8 is the whole rule; the lock-table entry is the same
// g_lockTable the unlock wrappers release (0x15A3's EnterCriticalSection, __lock itself is STALLED).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_lock8_1030(void) {
    EnterCriticalSection((LPCRITICAL_SECTION)(uintptr_t)g_lockTable[8 * 2]);
}

// 1.13c D2MCPClient+0x01039: the mirror 9-byte wrapper at 0x1039 (`push 8; call 0x14EE (_unlock);
// pop ecx; ret`), whose only caller is 0x016AE __onexit's failure path. Release exit lock 8.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock8_1039(void) {
    leave_cs(8);
}

// 1.13c D2MCPClient+0x016B4 (18 B, caller 0x0105A, callee 0x0167C __onexit): the VC7.1 `_atexit`.
// `p = __onexit(func); return (p != 0) - 1` -- 0 when the registration succeeded, -1 when __onexit
// answered null. The onexit table is module state and 0x0167C is UNMAPPED, so the callee is the seam.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_atexit_16b4(void* func, void* (*onexit)(void*)) {
    return (onexit(func) != nullptr) - 1;
}

// 1.13c D2MCPClient+0x0167C (50 B, caller 0x016B4 _atexit): the VC7.1 CRT `__onexit`. Retail: take
// exit lock 8 (0x01030), append `func` through __onexit_lk with the pointer in EDI (0x015D4), release
// exit lock 8 through 0x016AE -> 0x01039, and return __onexit_lk's result. The onexit table is module
// state owned by 0x015D4, still UNMAPPED, so the append is the seam.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_mcp_onexit_167c(void* func, void* (*onexit_lk)(void*)) {
    d2_mcp_lock8_1030();
    void* p = onexit_lk(func);
    d2_mcp_unlock8_1039();
    return p;
}

// 1.13c D2MCPClient+0x018A5 (9 B, caller 0x01AC5): `call TlsAlloc; ret`, the result discarded (the
// caller ignores it). wasm has no TLS indices, so there is nothing to allocate; same stand-in as
// d2ddraw:0x1B85 (src/core/gfx/d2ddraw_batch_10da.cpp).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_tlsalloc_18a5(void) {
}

// 1.13c D2MCPClient+0x016AE (6 B, caller 0x0167C __onexit): `call 0x01039; ret`, the onexit failure
// path's release of exit lock 8. The callee is the wrapper immediately above, so this is the same rule.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock8_16ae(void) {
    d2_mcp_unlock8_1039();
}

// 1.13c D2MCPClient+0x02B52 (9 B, caller 0x029EA _realloc): `push 4; call 0x14EE (_unlock); pop ecx;
// ret` -- release CRT lock 4. Same rule as the 0x01C07 wrapper above.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock4_2b52(void) {
    leave_cs(4);
}

// 1.13c D2MCPClient+0x02C09 (9 B, caller 0x02B9C __msize): the identical 9-byte `__unlock(4)` wrapper.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock4_2c09(void) {
    leave_cs(4);
}

// 1.13c D2MCPClient+0x02FFE (9 B, caller 0x02FCC ___updatetlocinfo): `push 0xC; call 0x14EE (_unlock);
// pop ecx; ret` -- release CRT lock 0xC.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock_c_2ffe(void) {
    leave_cs(0xc);
}

// 1.13c D2MCPClient+0x034D5 (9 B, caller 0x0338E __setmbcp): `push 0xD; call 0x14EE (_unlock); pop ecx;
// ret` -- release CRT lock 0xD.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock_d_34d5(void) {
    leave_cs(0xd);
}
