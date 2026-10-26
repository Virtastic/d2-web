// src/core/realm/mcp_crt_stalled.cpp -- 1.13c D2MCPClient.dll MSVC 2003 CRT leaves, from the cached decompile.
// Process exit maps to std::exit (the project's stand-in for retail fatal paths); the critical-section
// release maps to the host lock seam in mcp_tiny_rowex.cpp.
#include <cstdint>
#include <cstdlib>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" void d2_mcp_host_unlock(uint32_t idx);
extern "C" void d2_mcp_doexit(uint32_t code, uint32_t quick, uint32_t retcaller);

// 1.13c D2MCPClient+0x01000
// GetModuleHandle("mscoree.dll")/CorExitProcess has no host here; ExitProcess(code) is std::exit(code).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_crtExitProcess_1000(int code) { std::exit(code); }

// 1.13c D2MCPClient+0x01042
// Retail takes the table start in EAX and the end as the argument; call every non-null slot in order.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_initterm_1042(void (**cur)(void), void (**end)(void)) {
    for (; cur < end; ++cur)
        if (*cur) (*cur)();
}

// 1.13c D2MCPClient+0x01173
// The SEH epilogue reads the caller frame ([EBP+0x10]) and EDI; they arrive as arguments.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock8_cond_1173(int32_t frame_arg, int32_t edi) {
    if (frame_arg != edi) d2_mcp_host_unlock(8);
}

// 1.13c D2MCPClient+0x01198
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_exit_1198(int code) { d2_mcp_doexit((uint32_t)code, 1, 0); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_exit_1198_seam(int code, void (*doexit)(uint32_t, uint32_t, uint32_t)) { doexit((uint32_t)code, 1, 0); }

// 1.13c D2MCPClient+0x0141D
// error_mode/app_type (0xAA34/0xAA38) and the banner/message writers are host seams; then __exit(0xFF).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_amsg_exit_141d(int errnum, int error_mode, int app_type,
                                                          void (*banner)(void), void (*nmsg)(int)) {
    if (error_mode == 1 || (error_mode == 0 && app_type == 1)) { if (banner) banner(); }
    if (nmsg) nmsg(errnum);
    d2_mcp_exit_1198(0xff);
}

// 1.13c D2MCPClient+0x014EE
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock_14ee(uint32_t idx) { d2_mcp_host_unlock(idx); }

// 1.13c D2MCPClient+0x0159A
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock10_159a_seam(void (*unlock)(uint32_t)) { unlock(10); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock10_159a(void) { d2_mcp_unlock10_159a_seam(d2_mcp_unlock_14ee); }
