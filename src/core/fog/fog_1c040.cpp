// 1.13c Fog+0x1C040: returns the CPU time-stamp counter (rdtsc, EDX:EAX). Wasm has no rdtsc;
// a monotonic nanosecond clock stands in (only deltas are consumed, by Fog+0x1C740).
#include <chrono>
#include <cstdint>

extern "C" uint64_t fog_read_tsc_1c040() {  // 1.13c Fog+0x1C040
    return (uint64_t)std::chrono::steady_clock::now().time_since_epoch().count();
}
