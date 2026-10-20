// 1.13c D2Gfx+0x0670C: VC7.1 CRT locale monetary-string cleanup.
// Module globals and the _free callee are explicit seams; fields stay 32-bit retail pointers.
#include <cstdint>
#include <emscripten.h>

// monetary points at the record's +0x0C..+0x24 (seven fields); default_lconv likewise.
extern "C" EMSCRIPTEN_KEEPALIVE void d2gfx_free_lconv_mon(
    const uint32_t* lconv, const uint32_t* default_lconv,
    const uint32_t* static_mon, void (*free_field)(void*)) {
    if (lconv == nullptr) return;
    for (int i = 0; i < 7; ++i) {
        const uint32_t field = lconv[3 + i];
        if (field != default_lconv[3 + i] && field != static_mon[i])
            free_field(reinterpret_cast<void*>(static_cast<uintptr_t>(field)));
    }
}
