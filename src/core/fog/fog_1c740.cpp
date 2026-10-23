#include <cstdint>
// 1.13c Fog+0x1C740: CPU MHz estimate. Retail spins >=10 ms of GetTickCount between two
// (rdtsc, QueryPerformanceCounter) samples: MHz = dTSC * QPCfreq / dQPC / 1e6.
// Wasm has no rdtsc/thread priority; the arithmetic is the contract, samples are inputs.
int64_t fog_cpu_mhz_1c740(int64_t tsc0, int64_t tsc1, int64_t qpc0, int64_t qpc1, int64_t qpcFreq)
{
    int64_t dq = qpc1 - qpc0;
    if (dq == 0) return 0;
    return ((tsc1 - tsc0) * qpcFreq / dq) / 1000000;
}
