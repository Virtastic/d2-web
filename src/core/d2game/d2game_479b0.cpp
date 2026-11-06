// D2Game+0x479B0 -- create a unit and link it to its region owner.
// ecx = a, eax = b, stack (p1, p2) ret 8. Returns 1 when the unit was made, 0 otherwise.
//   u = 0x24950(eax 1, ecx p2, edx b; p1, a, 0, 0, -1)
//   0x23CA0(p1, u, 1); 0x248A0(eax 1, ecx b, edx p1, edi u; a, 3, 6); 0x47880(edi p1, ebx p2; b)
#include <cstdint>
#include <emscripten.h>

struct D2Game479B0Env {
    uint32_t (*sub24950)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5);
    void     (*sub23CA0)(uint32_t s1, uint32_t s2, uint32_t s3);
    void     (*sub248A0)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t edi, uint32_t s1, uint32_t s2, uint32_t s3);
    void     (*sub47880)(uint32_t edi, uint32_t ebx, uint32_t s1);
};

// 1.13c D2Game+0x479B0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_479b0_probe(uint32_t a, uint32_t b, uint32_t p1, uint32_t p2, const D2Game479B0Env* env)
{
    const uint32_t u = env->sub24950(1, p2, b, p1, a, 0, 0, 0xffffffffu);
    if (u == 0) return 0;
    env->sub23CA0(p1, u, 1);
    env->sub248A0(1, b, p1, u, a, 3, 6);
    env->sub47880(p1, p2, b);
    return 1;
}
