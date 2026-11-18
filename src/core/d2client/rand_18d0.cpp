#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x18D0: ECX = u64* seed, ESI = range, EDI = base. range < 1 -> base, seed untouched;
// else seed = lo*0x6AC690C5 + hi (u64), result = base + (pow2 ? lo & (range-1) : lo % range) of the new seed's low dword.
static uint64_t g_seed18d0;
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_18d0(uint32_t lo, uint32_t hi, int32_t range, int32_t base) {
    g_seed18d0 = ((uint64_t)hi << 32) | lo;
    if (range < 1) return (uint32_t)base;
    g_seed18d0 = (uint64_t)lo * 0x6AC690C5u + hi;
    const uint32_t n = (uint32_t)range, v = (uint32_t)g_seed18d0;
    return (uint32_t)base + ((n & (n - 1)) ? v % n : v & (n - 1));
}
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_18d0_seed(int32_t high) { return high ? (uint32_t)(g_seed18d0 >> 32) : (uint32_t)g_seed18d0; }
