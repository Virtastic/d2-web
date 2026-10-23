// src/core/d2win/d2win_entry_187e.cpp -- the 1.13c D2Win.dll PE entry (DllMain wrapper), from
// build-wasm/ghidra-d2win/decomp/0x187E.c. Same shape as d2client_entry_45f6.cpp:
//   reason 0 with no CRT attachment (+0xE1028 == 0) -> FALSE;
//   reasons 1/2 -> optional raw hook (+0x1BBC44), then __CRT_INIT@12 (RVA 0x16FD); either failing -> FALSE;
//   module body 0x10D70 (returns 1) runs next; failed body on reason 1 detaches __CRT_INIT(module, 0);
//   reasons 0/3 -> __CRT_INIT first, then the raw hook if it succeeded.
// __CRT_INIT is a host seam (DEAD row, MSVC CRT) that must maintain the attach count.
#include <cstdint>
#include <emscripten.h>

static uint32_t g_crt_refcount;                                          // DAT_6f901028
static int32_t (*g_crt_init)(uint32_t, int32_t);                         // RVA 0x016FD
static int32_t (*g_raw_dll_main)(uint32_t, int32_t, uint32_t);           // DAT_6f9abc44

extern "C" EMSCRIPTEN_KEEPALIVE void d2win_187e_seams(int32_t (*crt_init)(uint32_t, int32_t),
        int32_t (*raw)(uint32_t, int32_t, uint32_t), uint32_t refcount) {
    g_crt_init = crt_init; g_raw_dll_main = raw; g_crt_refcount = refcount;
}

// 1.13c D2Win+0x0187E
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_entry_187e(uint32_t module, int32_t reason, uint32_t reserved) {
    int32_t r = 1;
    if (reason == 0 && g_crt_refcount == 0) return 0;
    if (reason == 1 || reason == 2) {
        if (g_raw_dll_main) r = g_raw_dll_main(module, reason, reserved);
        if (r == 0 || (g_crt_init && g_crt_init(module, reason) == 0)) return 0;
    }
    r = 1;  // sub_10D70
    if (reason == 1 && r == 0 && g_crt_init) (void)g_crt_init(module, 0);
    if (reason == 0 || reason == 3) {
        if (g_crt_init && g_crt_init(module, reason) == 0) r = 0;
        if (r != 0 && g_raw_dll_main) r = g_raw_dll_main(module, reason, reserved);
    }
    return r;
}
