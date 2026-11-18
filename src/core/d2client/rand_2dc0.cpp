#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x2DC0: EAX = max, EDI = min, ECX = u64* seed. max <= min (signed) -> min, seed untouched;
// else n = max-min, seed = lo*0x6AC690C5 + hi (u64), result = min + (n pow2 ? v & (n-1) : v % n), v = new seed low dword.
static uint64_t g_seed2dc0;
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2dc0(uint32_t lo, uint32_t hi, int32_t max, int32_t min) {
    g_seed2dc0 = ((uint64_t)hi << 32) | lo;
    if (max <= min) return (uint32_t)min;
    const uint32_t n = (uint32_t)max - (uint32_t)min;
    g_seed2dc0 = (uint64_t)lo * 0x6AC690C5u + hi;
    const uint32_t v = (uint32_t)g_seed2dc0;
    return (uint32_t)min + ((n & (n - 1)) ? v % n : v & (n - 1));
}
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2dc0_seed(int32_t high) { return high ? (uint32_t)(g_seed2dc0 >> 32) : (uint32_t)g_seed2dc0; }
