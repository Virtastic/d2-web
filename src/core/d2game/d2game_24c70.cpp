// D2Game+0x24C70: create one record with 0x24950 and, on success, bind it (0x23CA0) and apply mods (0x248A0).
// EAX, ECX, EBX, ESI in registers, four stack words (ret 0x10). Callees arrive through the env.
#include <cstdint>
#include <emscripten.h>

struct D2Game24C70Env {
    uint32_t (*sub24950)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
    void     (*sub23CA0)(uint32_t s0, uint32_t s1, uint32_t s2);
    void     (*sub248A0)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2);
};

// 1.13c D2Game+0x24C70
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_24C70(uint32_t eax, uint32_t ecx, uint32_t ebx, uint32_t esi,
                                                            uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3,
                                                            const D2Game24C70Env* env)
{
    // 24950(eax=a3, ecx=a1, edx=ebx; esi, a0, cx, ax, -1)
    const uint32_t rec = env->sub24950(a3, a1, ebx, esi, a0, ecx & 0xffff, eax & 0xffff);
    if (rec == 0) return 0;
    env->sub23CA0(esi, rec, a2);
    env->sub248A0(1, ebx, esi, a0, 3, 6);
    return rec;
}
