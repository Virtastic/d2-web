// 1.13c Fog+0x3a7a: thunk, calls Fog+0x3C3F with the constant 0xC.
#include <cstdint>

struct FogSub3a7aEnv {
    void (*sub_3c3f)(int32_t arg);  // Fog+0x3C3F
};

extern "C" void fog_sub_3a7a(const FogSub3a7aEnv* e) { e->sub_3c3f(0xC); }
