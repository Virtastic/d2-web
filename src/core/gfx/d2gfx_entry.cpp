// src/core/gfx/d2gfx_entry.cpp -- the 1.13c D2Gfx.dll PE entry, recovered from
// build-wasm/ghidra-d2gfx/decomp/0x1807.c (208 B, base 0x6FA80000; row s3797).
//
// The row is MSVC 2003's VC7.1 `_DllMainCRTStartup`, the same 208-byte shape already recovered for
// D2DDraw (src/core/gfx/d2ddraw_batch_rowex.cpp:302), D2Client (src/core/d2client/d2client_entry_45f6.cpp)
// and D2MCPClient (src/core/realm/mcp_entry.cpp). It reads the CRT attach count (+0x10DE8), calls the
// optional raw DllMain callback (+0x1EA24), `__CRT_INIT@12` (0x1686) and the module's own DllMain
// (0xB160, `mov eax,1; ret 0xC` plus `ghWnd = 0` on process attach).
//
// 0x1686 is UNMAPPED and 0xB160 is its own UNMAPPED row, so both enter as seams after the retail
// arguments (the d2client/mcp entry pattern); the attach count lives in the env because 0x1686 owns it.
// The image leaves +0x1EA24 zero and no instruction in the module writes it (raw scan: only the two
// reads at 0x183B and 0x18AE), but the seam is kept because the retail branch tests it.
#include <cstdint>
#include <emscripten.h>

struct D2GfxEntryEnv {
    uint32_t* attach_count;                              // +0x10DE8 DAT_6fa90de8, owned by 0x01686
    int32_t (*raw_dll_main)(uint32_t, int32_t, uint32_t); // +0x1EA24 DAT_6fa9ea24, may be NULL
    int32_t (*crt_init)(uint32_t, int32_t);               // seam: __CRT_INIT@12 rva 0x01686
    int32_t (*dll_main)(uint32_t, int32_t, uint32_t);     // seam: module DllMain rva 0x0B160
};

// 1.13c D2Gfx+0x01807
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2gfx_entry(D2GfxEntryEnv* env, uint32_t module, int32_t reason,
                                         uint32_t reserved) {
    int32_t result = 1;
    if (reason == 0 && *env->attach_count == 0) return 0;   // detach with nothing attached
    if (reason == 1 || reason == 2) {                       // PROCESS_ATTACH / THREAD_ATTACH
        if (env->raw_dll_main != nullptr)
            result = env->raw_dll_main(module, reason, reserved);
        if (result == 0 || env->crt_init(module, reason) == 0) return 0;
    }
    result = env->dll_main(module, reason, reserved);
    if (reason == 1 && result == 0) (void)env->crt_init(module, 0);   // failed attach detaches the CRT
    if (reason == 0 || reason == 3) {                       // PROCESS_DETACH / THREAD_DETACH
        if (env->crt_init(module, reason) == 0) result = 0;
        if (result != 0 && env->raw_dll_main != nullptr)
            result = env->raw_dll_main(module, reason, reserved);
    }
    return result;
}
