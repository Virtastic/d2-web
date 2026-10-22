// src/core/d2sound/d2sound_crt_1510.cpp -- D2Sound.dll CRT helpers (lock stubs, calloc, __amsg_exit, __d_inttype).
// Locks are no-ops (single wasm thread), as in d2sound_crt_1000.cpp. Not ported: B2E9/CC84/C217/C349/C45A
// (x87 libm pow/log kernels and error dispatch through a CRT-private function pointer).
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <unistd.h>

// 1.13c D2sound+0x1510  _unlock(n): LeaveCriticalSection on lock n
void d2s_unlock(int) {}

// 1.13c D2sound+0x1525  __mtinitlocknum(n): allocates lock n's critical section; always succeeds here
int d2s_mtinitlocknum(int) { return 1; }

// 1.13c D2sound+0x1C45  _calloc_crt-style calloc(count, size): overflow-checked, zeroed
void* d2s_calloc(uint32_t count, uint32_t size) {
    if (count != 0 && size > 0xffffffe0u / count) return nullptr;
    return calloc(count ? count : 1, size ? size : 1);
}

// 1.13c D2sound+0x143F  __amsg_exit(msg): writes the runtime-error banner (no console here), exits 0xFF
[[noreturn]] void d2s_amsg_exit(int) { _exit(0xff); }

// 1.13c D2sound+0xC3EC  __d_inttype(x): 0 non-integer/denormal/NaN, 1 odd integer, 2 even integer
int d2s_d_inttype(double x) {
    if (std::fpclassify(x) == FP_SUBNORMAL) return 0;
    if (std::rint(x) != x) return 0;
    double h = x / 2.0;
    return std::rint(h) == h ? 2 : 1;
}
