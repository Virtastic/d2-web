// 1.13c D2Game+0xEC0E0: item-section write step of the save writer, injectable callee.
// EAX = end pointer, EDI = &cursor, ECX/EDX = inherited registers passed through, stack: a, b, c; ret 0xC.
// Retail pushes the inherited EDX, ECX as the callee's stack words 4-5, then loads ECX = c, EDX = a (and EBX = b) before
// calling 0x53B40(a, cur, end-cur, c, edx, ecx) (ret 0x18); negative result returns 3, else cursor += n and 0.
#include <cstdint>
struct GEC0E0Env {
    int32_t (*f53b40)(uint32_t regEcx, uint32_t regEdx, uint32_t regEbx, uint32_t a, uint32_t cur, uint32_t len, uint32_t c, uint32_t edx, uint32_t ecx);
};
// 1.13c D2Game+0xEC0E0
uint32_t g_ec0e0(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t edi, uint32_t a, uint32_t b, uint32_t c, const GEC0E0Env& env)
{
    uint32_t* pCur = (uint32_t*)(uintptr_t)edi;
    uint32_t cur = *pCur;
    int32_t n = env.f53b40(c, a, b, a, cur, eax - cur, c, edx, ecx);
    if (n < 0) return 3;
    *pCur = (uint32_t)n + cur;
    return 0;
}
