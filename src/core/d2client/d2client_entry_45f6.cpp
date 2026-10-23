// src/core/d2client/d2client_entry_45f6.cpp -- the 1.13c D2Client.dll PE entry (DllMain wrapper), recovered
// from build-wasm/ghidra-d2client/decomp/0x45F6.c (208 B, base 0x6FAB0000; row s3453). Same shape as the
// D2DDraw entry (src/core/gfx/d2ddraw_batch_rowex.cpp:302):
//   reason 0 with no CRT attachment -> FALSE;
//   reasons 1/2 -> optional raw DllMain hook (+0x124AC8, may be NULL), then __CRT_INIT(module, reason),
//                  either failing returns FALSE;
//   the module body 0x4BE80 runs next (stores the module, returns 1); a failed body on reason 1 detaches
//                  __CRT_INIT(module, 0);
//   reasons 0/3 -> __CRT_INIT(module, reason) first, then the raw hook again if it succeeded.
// The two stores the body makes (module -> +0x2C330) and the CRT attachment count (+0xFA8EC) are the only
// image globals read here. __CRT_INIT@12 (RVA 0x04475) is still UNMAPPED, so it enters as a host seam after
// the retail arguments (the d2client_batch_1e70.cpp pattern); the seam must maintain +0xFA8EC.
#include <cstdint>
#include <emscripten.h>

extern "C" uint32_t d2_client_4be80(uint32_t* g330, uint32_t a);

static uint32_t g_crt_refcount;                                         // +0xFA8EC DAT_6fbaa8ec, owned by 0x04475
static uint32_t g_module_2c330;                                         // +0x2C330 DAT_6fbcc330, written by 0x4BE80
static int32_t (*g_crt_init)(uint32_t module, int32_t reason);          // seam: __CRT_INIT@12 RVA 0x04475
static int32_t (*g_praw_dll_main)(uint32_t module, int32_t reason, uint32_t reserved); // +0x124AC8 DAT_6fbd4ac8

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_45f6_seams(int32_t (*crt_init)(uint32_t, int32_t),
                                               int32_t (*raw_dll_main)(uint32_t, int32_t, uint32_t)) {
    g_crt_init = crt_init;
    g_praw_dll_main = raw_dll_main;
}

// 1.13c D2Client+0x045f6
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_45f6(uint32_t module, int32_t reason, uint32_t reserved) {
    int32_t result = 1;
    if (reason == 0 && g_crt_refcount == 0) return 0;
    if (reason == 1 || reason == 2) {
        if (g_praw_dll_main) result = g_praw_dll_main(module, reason, reserved);
        if (result == 0) return 0;
        if (g_crt_init) {
            result = g_crt_init(module, reason);
            if (result == 0) return 0;
        }
    }
    result = (int32_t)d2_client_4be80(&g_module_2c330, module);
    if (reason == 1 && result == 0 && g_crt_init) (void)g_crt_init(module, 0);
    if (reason == 0 || reason == 3) {
        if (g_crt_init && g_crt_init(module, reason) == 0) result = 0;
        if (result != 0 && g_praw_dll_main) result = g_praw_dll_main(module, reason, reserved);
    }
    return result;
}
