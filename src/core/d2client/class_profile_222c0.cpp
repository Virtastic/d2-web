// 1.13c D2Client+0x222C0: EAX = class (0..6) -> the class profile pointer `0xF7694[class]`.
// Class >= 7: retail asserts (line 0xB22) and _exit(-1); the fatal path is not reproduced, returns 0.
#include <cstdint>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_222c0(uint32_t nClass, const uint32_t* classTbl) {
    return nClass < 7 ? classTbl[nClass] : 0;
}
