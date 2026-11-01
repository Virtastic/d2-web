// src/core/d2win/d2win_acf0.cpp -- 1.13c D2Win sub_ACF0 (export 10163), transcribed from the cached decompile.
#include <cstdint>
#include <cstdlib>

// __fastcall(ECX = control, EDX = value). Control type (first dword) must be 5, else the retail text-0x56
// report and _exit(-1). Stores max(value, 0) (signed) at control +0x48 and returns 1.
// 1.13c D2Win+0x0ACF0
int32_t d2win_acf0(uint32_t* ctl, int32_t v) {
    if (ctl[0] != 5) std::_Exit(-1);
    ctl[0x12] = v > 0 ? (uint32_t)v : 0;
    return 1;
}
