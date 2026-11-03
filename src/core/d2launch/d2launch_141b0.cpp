// 1.13c D2Launch+0x141B0 (sub_141B0) and D2Launch+0x141F0 (sub_141F0), stdcall(p), ret 4: gate on the 0x980E thunk (Fog #10227),
// then 0x121B0(*(p+0xc), tag1, tag2) with ESI = 5 (141B0: tags 0x2771, 0x57f6) or ESI = 6 (141F0: tags 0x2772, 0x57f7).
// ESI is set by the function itself (the callee reads it as a hidden register input); the caller's ESI is saved and restored.
#include <cstdint>

extern "C" uint32_t d2l_141b0_core(uint32_t esi, uint32_t tag1, uint32_t tag2, uint32_t p, int (*gate)(),
                                   void (*f)(uint32_t a, uint32_t t1, uint32_t t2, uint32_t esi)) {
    if (!gate()) return 0;
    f(*(const uint32_t*)(uintptr_t)(p + 0xc), tag1, tag2, esi);
    return 1;
}
