// 1.13c D2Win+0x18F25 (81 bytes): MSVC CRT __cfltcvt. Dispatch on the format character in the
// third slot ('e'/'E' -> 0x18D13, 'f' -> 0x18E1F, else -> 0x18E86). Float formatting itself lives in the callees (seams).
#include <cstdint>
#include <emscripten.h>

struct D2WinCfltcvtEnv {
    uint32_t (*fmtE)(void* arg, void* buf, int32_t a, int32_t b);   // 0x18D13
    uint32_t (*fmtF)(void* arg, void* buf, int32_t a);              // 0x18E1F
    uint32_t (*fmtG)(void* arg, void* buf, int32_t a, int32_t b);   // 0x18E86
};

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_18F25(void* arg, void* buf, uint32_t fmtChar, int32_t a, int32_t b,
                         const D2WinCfltcvtEnv* e) {
    if (fmtChar == 0x65 || fmtChar == 0x45) return e->fmtE(arg, buf, a, b);
    if (fmtChar == 0x66) return e->fmtF(arg, buf, a);
    return e->fmtG(arg, buf, a, b);
}
