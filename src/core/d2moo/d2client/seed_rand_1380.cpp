#include <cstdint>
#include <emscripten.h>

namespace { uint32_t g_lo1380 = 0, g_hi1380 = 0; }

// 1.13c D2Client+0x1380: LCG on a {lo,hi} seed (ECX): seed = lo*0x6AC690C5 + hi (64-bit); returns
// n<1 -> 0 (seed untouched), n pow2 -> (n-1)&lo32, else lo32 % n.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1380(uint32_t lo, uint32_t hi, int32_t n) {
    g_lo1380 = lo; g_hi1380 = hi;
    if (n < 1) return 0;
    const uint64_t s = static_cast<uint64_t>(lo) * 0x6AC690C5u + hi;
    g_lo1380 = static_cast<uint32_t>(s); g_hi1380 = static_cast<uint32_t>(s >> 32);
    const uint32_t un = static_cast<uint32_t>(n);
    return (un & (un - 1)) ? g_lo1380 % un : (un - 1) & g_lo1380;
}
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1380_lo() { return g_lo1380; }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1380_hi() { return g_hi1380; }
