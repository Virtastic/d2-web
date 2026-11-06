// 1.13c D2Game+0xB06F0: ECX = value, EDX = record {fn, ecx, edx, word}, out slot; call only while *out is zero.
#include <cstdint>
struct GB06f0Env { uint32_t (*call)(uint32_t fn, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1); };
void g_b06f0(uint32_t ecx, uint32_t rec, uint32_t out, const GB06f0Env& env) {
    const uint32_t* r = (const uint32_t*)(uintptr_t)rec;
    uint32_t* o = (uint32_t*)(uintptr_t)out;
    if (*o == 0) {
        uint32_t v = env.call(r[0], r[1], r[2], ecx, r[3]);
        if (v != 0) *o = v;
    }
}
