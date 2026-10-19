// src/core/realm/mcp_crt_mtterm.cpp -- 1.13c D2MCPClient.dll __mtterm, from the cached decompile
// (build-wasm/ghidra-d2mcpclient/decomp/0x18AE.c). Stage A of the row: implementation + marker only.
// Retail base 0x6FA20000.
#include <cstdint>
#include <emscripten.h>

extern "C" void d2_fn_d2mcpclient_1499(void);   // __mtdeletelocks, src/core/d2mcpclient/

// 1.13c D2MCPClient+0x018AE (29 B): +0xA158 is the Fls/Tls index and +0xAB9C is FlsFree/TlsFree.
// Pass the module-state slot and resolved import as seams; the free result is ignored by retail.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_mtterm(uint32_t* tls_index, int32_t (*tls_free)(uint32_t)) {
    if (*tls_index != 0xFFFFFFFFu) {
        (void)tls_free(*tls_index);
        *tls_index = 0xFFFFFFFFu;
    }
    d2_fn_d2mcpclient_1499();
}
