#include <cstdint>
#include <cstdlib>
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
// (unbacked .data), so both `quick` branches call nothing and the only writes are the three globals.
//
// Our engine has no process to terminate and never runs this CRT shutdown, so this is a shim: it models
// the two flag writes and the `retcaller` branch, not `TerminateProcess` / `___crtExitProcess` (no wasm
// analogue) nor the empty onexit/`__initterm` walks (nothing to call), which is why the row's vector
// records the three globals only.
static uint32_t g_C_Termination_Done;   // +0xAA20
static uint32_t g_C_Exit_Done;          // +0xAA24
static uint8_t  g_exitflag;             // +0xAA1C

extern "C" void d2_mcp_doexit(uint32_t code, uint32_t quick, uint32_t retcaller) {
    (void)quick;                        // both quick branches are empty walks in this image
    if (g_C_Exit_Done == 1) return;     // retail: TerminateProcess(GetCurrentProcess(), code)
    g_C_Termination_Done = 1;
    g_exitflag = (uint8_t)retcaller;    // byte store
    (void)code;                         // code reaches only the terminating (unmodelled) paths
    if (retcaller != 0) return;         // ___crtExitProcess not reached
    g_C_Exit_Done = 1;
}

// The probe reads the shim's CRT state back out (the vector records these globals).
extern "C" void d2_mcp_doexit_snapshot(uint32_t* s) {
    s[0] = g_C_Termination_Done; s[1] = g_C_Exit_Done; s[2] = g_exitflag;
}
