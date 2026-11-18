// 1.13c D2Client+0x7C650 (50 B, no arguments): release the object at global D2Client+0x113A60. A set pointer runs 0x7B3D0
// (EAX = the word at obj+0), then the Fog free thunk 0xBF86 (ECX = obj, EDX = the 0xD4BDC source string, stack line 0x68A and 0),
// then clears the global. A null global does nothing.
#include <cstdint>
#include <emscripten.h>

struct D2c7C650Env {
    void (*freeList)(uint32_t first);                                                       // 0x7B3D0 (EAX)
    void (*fogFree)(uint32_t p, uint32_t sourceRva, uint32_t line, uint32_t zero);          // 0xBF86 (ECX, EDX, ret 8)
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_port_7c650(uint32_t* g_113a60, const D2c7C650Env* env) {
    const uint32_t obj = *g_113a60;
    if (!obj) return;
    env->freeList(*(const uint32_t*)(uintptr_t)obj);
    env->fogFree(obj, 0xD4BDC, 0x68a, 0);
    *g_113a60 = 0;
}
