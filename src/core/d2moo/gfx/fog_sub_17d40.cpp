#include <cstdint>
#include <emscripten.h>

namespace {
uint32_t (*g_write_file)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
}

extern "C" void fog_sub_17D40_seams(uint32_t (*write_file)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)) {
    g_write_file = write_file;
}

// 1.13c Fog+0x17D40 (10111, 26 B, __fastcall)
// Thunk to WriteFile(handle, buf, len, pWritten, overlapped); BOOL result discarded (void return).
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_17D40(uint32_t h, uint32_t buf, uint32_t len, uint32_t written,
                                                   uint32_t ovl) {
    if (g_write_file) g_write_file(h, buf, len, written, ovl);
}
