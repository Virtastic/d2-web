// src/core/d2sound/d2sound_batch_27.cpp -- D2Sound batch 27 (CRT + DirectSound init).
#include <cstdint>

void d2s_unlock(int);

// Web audio stand-in for the DirectSound bring-up (the retail routine is ported in d2sound_93a0.cpp); a no-op here.
void d2s_dsound_init() {}

// 1.13c D2sound+0xACCC  _ftol: x87 ST0 -> int64 truncated toward zero (fistp, then a one-step correction by the sign of the
// remainder). Out of range or NaN, fistp stores the integer indefinite 0x8000000000000000, which retail returns unchanged.
int64_t d2s_ftol(double x) {
    if (!(x >= -9223372036854775808.0 && x < 9223372036854775808.0)) return INT64_MIN;
    return (int64_t)x;
}

// 1.13c D2sound+0x260C  _FF_MSGBANNER-style runtime error writer (console write or MessageBox); no UI here
void d2s_rterr_write(int) {}

// (superseded by d2sound_n95.cpp) D2sound 0x1C27  _unlock(4)
__attribute__((weak)) void d2s_unlock4() { d2s_unlock(4); }
