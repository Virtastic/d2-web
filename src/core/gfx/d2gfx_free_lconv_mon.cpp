// 1.13c D2Gfx+0x0670C: MSVC locale monetary-string cleanup.
// D2Gfx's decompile visits seven pointer fields (+0x0c through +0x24), unlike
// the eight-field BNClient CRT variant. Module-owned defaults and allocator
// state arrive as seams after the retail lconv argument.
#include <cstdint>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE
void d2gfx_free_lconv_mon(const uint32_t* lconv, const uint32_t* default_lconv,
                           const uint32_t* static_mon, void (*free_ptr)(void*)) {
    if (!lconv) return;
    for (int i = 3; i <= 9; ++i) {
        const uint32_t ptr = lconv[i];
        if (ptr != default_lconv[i] && ptr != static_mon[i - 3]) {
            free_ptr(reinterpret_cast<void*>(static_cast<uintptr_t>(ptr)));
        }
    }
}
