// src/core/realm/mcp_entry.cpp -- one UNMAPPED 1.13c D2MCPClient.dll row reimplemented from the
// decompile in build-wasm/ghidra-d2mcpclient/decomp/0x1339.c (alpha lane). Base 0x6FA20000. Prior art:
// the same VC7.1 `_DllMainCRTStartup` already recovered for BNClient in
// src/core/realm/bnclient_batch_1af0.cpp (0x215E, also 208 bytes) -- the rule is identical.
//
// The row is MSVC 2003's linker-provided DllMain wrapper. It reads the CRT attach count (+0xAA28), calls
// the module callback (+0x125C4 DAT_6fa325c4, `__pRawDllMain`, may be NULL) and `__CRT_INIT@12` (0x11B8,
// owned by mcp_crt_init.cpp -- a seam here), and the module's own DllMain (0x7600). 0x7600 is eight bytes
// with no arguments and no callees: the default `_DllMain@12`, `mov eax,1; ret 0Ch`, i.e. constant 1
// (already recovered as src/core/fn/tiny_ret_const2.cpp:d2_fn_d2mcpclient_7600). The two SEH helpers the
// decompile lists, 0x1750/0x178B, are compiler-prologue glue (both STALLED: no wasm analogue).
//
// Everything else is host/module state no argument pins, so the callback, the CRT init, its opaque env
// and the shared attach count are seams in D2McpEntryEnv.
#include <cstdint>
#include <emscripten.h>

struct D2McpEntryEnv {
    uint32_t* attach_count;                             // +0xAA28 DAT_6fa2aa28, shared with the CRT env
    int32_t (*dll_callback)(void*, uint32_t, uint32_t); // +0x125C4 DAT_6fa325c4, may be null
    void* crt_env;                                      // D2McpCrtInitEnv* for the 0x11B8 seam
    int32_t (*crt_init)(void*, uint32_t, uint32_t, void*);// retail D2MCPClient rva 0x11B8
};

// 1.13c D2MCPClient+0x01339
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_entry(D2McpEntryEnv* env, uint32_t hinst, int32_t reason,
                                          uint32_t reserved) {
    int32_t result = 1;
    if (reason == 0 && *env->attach_count == 0) return 0;   // detach with nothing attached
    if (reason == 1 || reason == 2) {                       // PROCESS_ATTACH / THREAD_ATTACH
        if (env->dll_callback != nullptr)
            result = env->dll_callback((void*)(uintptr_t)hinst, (uint32_t)reason, reserved);
        if (result == 0 || env->crt_init((void*)(uintptr_t)hinst, (uint32_t)reason, reserved,
                                         env->crt_env) == 0)
            return 0;
    }
    result = 1;                                             // 0x7600: default _DllMain@12 -> TRUE
    if (reason == 1 && result == 0)                         // unreachable: kept for 1:1 with retail
        (void)env->crt_init((void*)(uintptr_t)hinst, 0, reserved, env->crt_env);
    if (reason == 0 || reason == 3) {                       // PROCESS_DETACH / THREAD_DETACH
        if (env->crt_init((void*)(uintptr_t)hinst, (uint32_t)reason, reserved, env->crt_env) == 0)
            result = 0;
        if (result != 0 && env->dll_callback != nullptr)
            result = env->dll_callback((void*)(uintptr_t)hinst, (uint32_t)reason, reserved);
    }
    return result;
}
