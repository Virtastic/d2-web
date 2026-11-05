// 1.13c D2Game+0xDAA40, injectable callee. EAX = a, ECX = b, EDX = c (plain ret). The two 11-entry ladders sit in
// retail's .rdata at 0xFA01C (a >= b) and 0xFA048 (a < b); an entry of 0x100 means "no change" (returns c). The
// scaling callee 0x14D0 takes EAX, ECX, EDX in registers and returns a * c / b.
#include <cstdint>
struct GDAA40Env {
    int32_t (*f14d0)(int32_t eax, int32_t ecx, int32_t edx);
};
static const int32_t kLadderGe[11] = { 256, 256, 256, 256, 256, 256, 207, 159, 110, 61, 13 };   // 0xFA01C
static const int32_t kLadderLt[11] = { 256, 256, 256, 256, 256, 256, 225, 174, 92, 38, 5 };     // 0xFA048
// 1.13c D2Game+0xDAA40
int32_t g_daa40(int32_t eax, int32_t ecx, int32_t edx, const GDAA40Env& env)
{
    int32_t t;
    if (ecx > eax) {
        if (eax >= 0x19 && ecx > 0) return env.f14d0(eax, ecx, edx);
        int32_t d = ecx - eax;
        if (d >= 0xB) d = 10;
        t = kLadderLt[d];
    } else {
        int32_t d = eax - ecx;
        if (d >= 0xB) d = 10;
        t = kLadderGe[d];
    }
    if (t == 0x100) return edx;
    return env.f14d0(t, 0x100, edx);
}
