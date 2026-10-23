// src/core/d2win/d2win_sub_137b0.cpp -- D2Win.dll sub_137B0: control validity check. Callees are seams.
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

struct D2WinSub137b0Env {
    uint32_t (*get_string)(uint32_t id);                  // Ordinal_10265
    void (*report)(const char* file, uint32_t msg);       // Ordinal_10024
};

// 1.13c D2Win+0x137B0
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_137b0(const D2WinSub137b0Env* e, const int32_t* ctl) {
    uint32_t id = 0x198;
    if (ctl) {
        if (ctl[0] == 1) return (ctl[0x15] != -1 && ctl[0x15] != ctl[0x16]) ? 1 : 0;
        id = 0x19a;
    }
    e->report("", e->get_string(id));
    exit(-1);
}
