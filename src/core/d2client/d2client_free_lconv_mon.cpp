// 1.13c D2Client+0x0AEE4: ___free_lconv_mon
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

struct D2ClientLconvMon {
    uint32_t unused[3];
    uint32_t monetary[7];
};
static_assert(offsetof(D2ClientLconvMon, monetary) == 0x0C);
static_assert(offsetof(D2ClientLconvMon, monetary[6]) == 0x24);
static_assert(sizeof(D2ClientLconvMon) == 0x28);

using D2ClientFreeFn = void (*)(uint32_t pointer, void* context);

// The defaults and allocator are seams for the module globals and _free side effect.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_free_lconv_mon(
    const D2ClientLconvMon* lconv, const uint32_t* defaults,
    const uint32_t* static_mon, D2ClientFreeFn free_ptr, void* context) {
    if (lconv == nullptr) return;
    for (uint32_t i = 0; i < 7; ++i) {
        const uint32_t ptr = lconv->monetary[i];
        if (ptr == defaults[i + 3] || ptr == static_mon[i]) continue;
        free_ptr(ptr, context);
    }
}
