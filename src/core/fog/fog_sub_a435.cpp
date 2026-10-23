// 1.13c Fog+0xA435: thunk, calls Fog+0x3C3F with the constant 10.
#include <cstdint>

struct FogA435Env { void (*sub_3c3f)(int32_t); };

extern "C" void fog_sub_a435(const FogA435Env* env) {  // 1.13c Fog+0xA435
    env->sub_3c3f(10);
}
