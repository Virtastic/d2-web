// src/core/realm/mcp_crt_batch_b52.cpp -- five UNMAPPED 1.13c D2MCPClient.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2mcpclient/decomp/0x<RVA>.c (alpha lane). Base 0x6FA20000; every
// global is named at its retail RVA. These are MSVC 2003 (VC7.1) Release CRT leaves: the per-thread
// data seeding of __CRT_INIT's thread-attach path, the two _unlock lock-number thunks, the heap-kind
// selector __heap_init reads, and the C++ EH filter thunk. Host services are seams after the retail
// arguments, which keep their order (src/core/realm/mcp_crt_mem.cpp pattern). The byte-identical
// D2Game (src/core/d2game/d2game_crt_b23.cpp 0x3B80) and BNClient (src/core/realm/bnclient_batch_rowex.cpp
// 0x3255) __initptd bodies are the cross-check for this one; the D2DDraw copies (0x261D heap_select,
// 0x22ED batch) for that pair.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2MCPClient+0x018CB (19 B, caller 0x011B8 __CRT_INIT): __initptd(ptd, locale). Seed the
// caller's _tiddata: _initaddr +0x54 = the retail VA 0x6FA2A190 (stored as its RVA 0xA190, the port's
// convention for retail VAs) and _holdrand +0x14 = 1. `locale` is not touched.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_initptd(uint8_t* ptd, void* locale) {
    (void)locale;
    *(uint32_t*)(ptd + 0x54) = 0xA190u;   // _initaddr, retail 0x6FA2A190
    *(uint32_t*)(ptd + 0x14) = 1u;        // _holdrand
}

// The lock release at 0x14EE -- LeaveCriticalSection on (&DAT_6FA2A038)[lock_num * 2] -- is a host seam.
typedef void (*D2McpUnlockFn)(int32_t lock_num);

// 1.13c D2MCPClient+0x01A81 (9 B, caller 0x0194F __freefls): the __unlock(0xD) thunk -- push 0xD,
// call the lock release at 0x14EE, pop, ret.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock_d(D2McpUnlockFn unlock) { unlock(0xd); }

// 1.13c D2MCPClient+0x01A8D (9 B, caller 0x0194F __freefls): the __unlock(0xC) thunk, same shape.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_unlock_c(D2McpUnlockFn unlock) { unlock(0xc); }

// 1.13c D2MCPClient+0x0233D (26 B, caller 0x02357 __heap_init): ___heap_select. On NT5 and later -- the
// OS platform word DAT_6FA2A9E4 == 2 (VER_PLATFORM_WIN32_NT) and the major version DAT_6FA2A9F0 > 4 --
// the CRT uses the OS heap and returns 1; anything else selects the small-block heap and returns 3.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_heap_select(uint32_t os_platform, uint32_t win_major) {
    return (os_platform == 2 && win_major > 4) ? 1 : 3;
}

// 1.13c D2MCPClient+0x025D1 (27 B, no direct caller -- reached through the module's SEH handler data):
// the C++ exception filter thunk. Only exception code 0xE06D7363 (MSVC C++ EH, `cmp [esp+4],eax`) is
// forwarded, unchanged, to __XcptFilter 0x0246D together with the EXCEPTION_POINTERS; every other code
// returns 0 without calling it.
typedef int32_t (*D2McpXcptFilterFn)(uint32_t xcptnum, void* exception_ptrs);

extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_cxx_eh_filter(int32_t exception_code, void* exception_ptrs,
                                                  D2McpXcptFilterFn xcpt_filter) {
    if ((uint32_t)exception_code != 0xE06D7363u) return 0;
    return xcpt_filter(0xE06D7363u, exception_ptrs);
}
