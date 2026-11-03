// 1.13c D2Client+0xA1A50: unit scan with the 0x509F0 filter. EAX = flags (0 selects 0x583), ECX = a, EDX = b, EDI = out
// pointer (may be null), stack s0,s1,s2 (ret 0xC). Builds the local block L[6] = {0,-1,0,a,a,0}, calls the scanner
// 0xA1860(s0,s1,s2,b,flags|0xA280,filter 0x509F0,&L,0) (8 stack words, ret 0x20), returns L[0] if non-zero else L[2],
// and stores L[5] through EDI when EDI is non-null.
#include <cstdint>
#include <emscripten.h>

struct D2cA1a50Env {
    // 0xA1860: the six-dword local block is the out-parameter; filterRva is 0x509F0 (the callback's D2Client RVA).
    void (*scan)(uint32_t s0, uint32_t s1, uint32_t s2, uint32_t b, uint32_t flags, uint32_t filterRva, uint32_t* L, uint32_t zero);
};

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a1a50(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1,
                                                         uint32_t s2, uint32_t* edi, const D2cA1a50Env* e) {
    uint32_t L[6] = {0, 0xFFFFFFFFu, 0, ecx, ecx, 0};
    if (eax == 0) eax = 0x583;
    e->scan(s0, s1, s2, edx, eax | 0xA280, 0x509F0, L, 0);
    uint32_t r = L[0] ? L[0] : L[2];
    if (edi) *edi = L[5];
    return r;
}
