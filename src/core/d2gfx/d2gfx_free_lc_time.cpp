#include <cstddef>
#include <cstdint>

// Retail slots contain x86 pointer values; the host resolves and frees each token.
struct D2GfxLcTimeStrings {
    uint32_t abbreviated_days[7];
    uint32_t days[7];
    uint32_t remaining[29];
};
static_assert(offsetof(D2GfxLcTimeStrings, days) == 0x1C);
static_assert(offsetof(D2GfxLcTimeStrings, remaining) == 0x38);
static_assert(sizeof(D2GfxLcTimeStrings) == 0xAC);

// 1.13c D2Gfx+0x0651D
extern "C" void d2gfx_free_lc_time(const D2GfxLcTimeStrings* time,
                                  void (*free_token)(uint32_t)) {
    if (time == nullptr) return;
    for (unsigned i = 1; i < 7; ++i) free_token(time->abbreviated_days[i]);
    free_token(time->abbreviated_days[0]);
    for (unsigned i = 1; i < 7; ++i) free_token(time->days[i]);
    free_token(time->days[0]);
    for (unsigned i = 0; i < 29; ++i) free_token(time->remaining[i]);
}
