// 1.13c D2Gfx+0x066AD: VC7.1 CRT locale numeric-string cleanup.
// Retail's module globals and _free callee are explicit seams; the three fields
// remain 32-bit retail pointers even in the wasm64 build.
#include <cstdint>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE void d2gfx_free_lconv_num(
    const uint32_t* numeric, const uint32_t* default_lconv,
    const uint32_t* static_numeric, void (*free_field)(void*)) {
    if (numeric == nullptr) return;
    for (int i = 0; i < 3; ++i) {
        const uint32_t field = numeric[i];
        if (field != default_lconv[i] && field != static_numeric[i])
            free_field(reinterpret_cast<void*>(static_cast<uintptr_t>(field)));
    }
}
