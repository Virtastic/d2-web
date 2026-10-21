#include <cstdint>

// Retail slots hold x86 pointer values; the host resolves and frees each token.
// Free order: 1..6, 0, 8..13, 7, 14..42.
// 1.13c D2Common+0x0668D
extern "C" void d2common_free_lc_time(const uint32_t* time, void (*free_token)(uint32_t)) {
    if (time == nullptr) return;
    for (unsigned i = 1; i < 7; ++i) free_token(time[i]);
    free_token(time[0]);
    for (unsigned i = 8; i < 14; ++i) free_token(time[i]);
    free_token(time[7]);
    for (unsigned i = 14; i < 43; ++i) free_token(time[i]);
}
