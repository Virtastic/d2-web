// src/core/realm/mcp_crt_mtterm.cpp -- 1.13c D2MCPClient.dll __mtterm, from the cached decompile
// (build-wasm/ghidra-d2mcpclient/decomp/0x18AE.c). Stage A of the row: implementation + marker only.
// Retail base 0x6FA20000.
#include <cstdint>
#include <emscripten.h>

// +0x2158 DAT_6fa2a158: the CRT's current locale, -1 when detached (__freeptd/__initptd own it in
// retail; no init path is implemented for this row yet).
static uintptr_t g_mcpLocale = (uintptr_t)-1;

// +0x2AB9C DAT_6fa2ab9c: the CRT free kernel the heap init installs. A host seam under wasm.
static void (*g_mcpFree)(void*) = nullptr;

extern "C" void d2_mcp_mtterm_set_kernels(uintptr_t locale, void (*free_fn)(void*)) {
    g_mcpLocale = locale;
    g_mcpFree = free_fn;
}

extern "C" void d2_fn_d2mcpclient_1499(void);   // __mtdeletelocks, src/core/d2mcpclient/

// 1.13c D2MCPClient+0x018AE
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_mtterm(void) {
    if (g_mcpLocale != (uintptr_t)-1) {
        g_mcpFree((void*)g_mcpLocale);
        g_mcpLocale = (uintptr_t)-1;
    }
    d2_fn_d2mcpclient_1499();
}
