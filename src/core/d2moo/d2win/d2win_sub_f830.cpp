// src/core/d2win/d2win_sub_f830.cpp -- 1.13c D2Win.dll ordinal 10097 (0x0F830, 40 B), transcribed from
// the cached Ghidra decompile. Alpha lane: implementation + marker only, no spec, no vector.
#include <cstdint>
#include <unistd.h>

// Host seam for the fatal path (Ordinal_10265(0x72) fetches the message, Ordinal_10024("", msg) reports it).
struct D2WinSubF830Env { void (*fatal)(uint32_t string_id); };
static void d2win_f830_fatal_default(uint32_t) {}

// 1.13c D2Win+0x0F830: control (ECX) must be type 6, else report string 0x72 and _exit(-1); then set
// both dwords at +0x44 and +0x48 to value (EDX).
extern "C" void d2win_sub_f830(uint32_t* ctrl, uint32_t value, const D2WinSubF830Env* env) {
    if (ctrl[0] != 6) {
        (env ? env->fatal : d2win_f830_fatal_default)(0x72);
        _exit(-1);
    }
    ctrl[0x11] = value;
    ctrl[0x12] = value;
}
