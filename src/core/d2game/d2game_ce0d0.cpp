// 1.13c D2Game+0xCE0D0 (size 68, ret 8): ECX = this, EDX = arg, two stack words (w0, w1).
// 0xCBF50 gets (w0 value, EDX, this, w1, &w0, &w1) and may rewrite both words through the pointers.
// If it returns 0, 0x78550 runs in mode 2 with this, EDX, 0, 2, low 16 bits of w0, low 16 bits of w1, 0,
// and the function returns 0; otherwise it returns the 0xCBF50 result.
// GCe040Env is the same struct the 0xCE040 probe declares (the mangled name of g_ce0d0 uses it).
#include <cstdint>
struct GCe040Env {
    uint32_t (*cbf50)(uint32_t eax, uint32_t esi, uint32_t s0, uint32_t s1, uint32_t* s2, uint32_t* s3);
    void (*f78550)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
};
uint32_t g_ce0d0(uint32_t ecx, uint32_t edx, uint32_t pkt, uint32_t len, const GCe040Env& env) {
    uint32_t w0 = pkt, w1 = len;
    uint32_t r = env.cbf50(w0, edx, ecx, w1, &w0, &w1);
    if (r != 0) return r;
    env.f78550(ecx, edx, 0, 2, w0 & 0xffff, w1 & 0xffff, 0);
    return 0;
}
