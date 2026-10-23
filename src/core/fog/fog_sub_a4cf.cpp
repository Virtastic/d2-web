// 1.13c Fog+0xA4CF: thunk, calls Fog+0x3C3F with 0xB.
#include <cstdint>

struct FogA4cfEnv {
    void (*sub_3c3f)(int32_t arg);  // Fog+0x3C3F
};

extern "C" void fog_sub_a4cf(const FogA4cfEnv* e) { e->sub_3c3f(0xb); }
