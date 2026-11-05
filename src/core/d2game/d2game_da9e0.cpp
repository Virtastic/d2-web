// 1.13c D2Game+0xDA9E0, injectable callees. EAX = value, stack word 0 = id (ret 4). Scales the value by the
// fixed-point factor of ordinal 10989(id), whose scale shift is ordinal 10989(0); saturates the product's
// precision instead of overflowing. Retail's ordinal 10989 takes one stack word (the thunk is at 0xA784).
#include <cstdint>
struct GDA9E0Env {
    int32_t (*o10989)(uint32_t id);
};
// 1.13c D2Game+0xDA9E0
int32_t g_da9e0(int32_t eax, uint32_t id, const GDA9E0Env& env)
{
    int32_t v = eax;
    if (v <= 0) return v;
    const int32_t f = env.o10989(id);
    const int32_t sh = env.o10989(0);
    if (sh > 0 && sh < 0x20) {
        const uint32_t cl = (uint32_t)(((f >> sh) + sh) & 0x1F);   // sar edx,cl ; add edx,eax ; sar ebx,cl uses the low 5 bits
        const int32_t lim = (int32_t)(0x7FFFFFFF >> cl);
        if (v > lim) return (int32_t)((uint32_t)(v >> sh) * (uint32_t)f);
        return (int32_t)((uint32_t)f * (uint32_t)v) >> sh;
    }
    return v;
}
