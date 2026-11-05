// 1.13c D2Client+0x7C730 (51 B, no arguments): tear down two UI globals. A set window word at D2Client+0x113A70 runs 0xA3D20
// (no register inputs) and clears; a set object pointer at +0x113A74 runs its virtual slot 0 (this = obj, one stack word, 1:
// the scalar deleting destructor) and clears.
#include <cstdint>
#include <emscripten.h>

struct D2c7C730Env {
    void (*closeWindow)();                          // 0xA3D20
    void (*destroyObject)(uint32_t obj, uint32_t flag);   // [[obj]+0] (thiscall, ret 4)
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_port_7c730(uint32_t* g_113a70, uint32_t* g_113a74, const D2c7C730Env* env) {
    if (*g_113a70) {
        env->closeWindow();
        *g_113a70 = 0;
    }
    const uint32_t obj = *g_113a74;
    if (obj) {
        env->destroyObject(obj, 1);
        *g_113a74 = 0;
    }
}
