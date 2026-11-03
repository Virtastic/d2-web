// 1.13c D2Client+0xA1AD0: unit scan with the 0x50740 filter. EAX = flags, ECX = a (unused after the local push),
// EDX = b, stack s0,s1,s2 (ret 0xC). One local dword L = 0; calls 0xA1860(s0,s1,s2,b,flags|0xA783,filter 0x50740,&L,0)
// and returns L.
#include <cstdint>
#include <emscripten.h>

struct D2cA1ad0Env {
    void (*scan)(uint32_t s0, uint32_t s1, uint32_t s2, uint32_t b, uint32_t flags, uint32_t filterRva, uint32_t* L, uint32_t zero);
};

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a1ad0(uint32_t eax, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2,
                                                         const D2cA1ad0Env* e) {
    uint32_t L = 0;
    e->scan(s0, s1, s2, edx, eax | 0xA783, 0x50740, &L, 0);
    return L;
}
