#include <cstdint>
#include <emscripten.h>

namespace {
uint32_t (*g_create_file)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
}

extern "C" void fog_sub_17DA0_seams(
    uint32_t (*create_file)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)) {
    g_create_file = create_file;
}

// 1.13c Fog+0x17DA0 (10107, 36 B, __fastcall)
// Thunk to CreateFileA with the seven arguments passed through; the HANDLE result is discarded
// (void return), so the call's side effect is the whole contract.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_17DA0(uint32_t name, uint32_t access, uint32_t share,
                                                   uint32_t sec, uint32_t disp, uint32_t flags,
                                                   uint32_t tmpl) {
    if (g_create_file) g_create_file(name, access, share, sec, disp, flags, tmpl);
}
