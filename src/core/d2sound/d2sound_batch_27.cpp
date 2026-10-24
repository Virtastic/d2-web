// src/core/d2sound/d2sound_batch_27.cpp -- D2Sound batch 27 (CRT + DirectSound init).
#include <cstdint>

void d2s_unlock(int);

// 1.13c D2sound+0x93A0  DirectSound bring-up (CreateDirectSound, SetCooperativeLevel, primary buffer 22050/16/stereo,
// EAX probe, then the 3D/volume/channel init and flag 6588=1). Web audio replaces DirectSound; init is a no-op here.
void d2s_dsound_init() {}

// 1.13c D2sound+0xACCC  _ftol: x87 ST0 -> int64 truncated toward zero
int64_t d2s_ftol(double x) { return (int64_t)x; }

// 1.13c D2sound+0x260C  _FF_MSGBANNER-style runtime error writer (console write or MessageBox); no UI here
void d2s_rterr_write(int) {}

// (superseded by d2sound_n95.cpp) D2sound 0x1C27  _unlock(4)
__attribute__((weak)) void d2s_unlock4() { d2s_unlock(4); }
