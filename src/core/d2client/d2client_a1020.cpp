// 1.13c D2Client+0xA1020: EAX = unit, EBX = optional pointer, stack s0..s4 (ret 0x14). Builds a 23-dword request record
// (+0 = 3, |4 with EBX) from the unit and hands it to D2Client+0xA0DB0 (ESI = record, stack = 0 or a copy with word 4 := s2).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cA1020Env {
    uint32_t (*ord10686)(uint32_t p);                              // thunk C3BE
    uint32_t (*ord10476)(uint32_t p);                              // thunk C2EC
    void (*a0db0)(const uint32_t* rec, const uint32_t* copy);      // ESI = rec, stack word = copy or 0 (ret 4)
};
extern "C" {
EMSCRIPTEN_KEEPALIVE void d2c_a1020(const D2cA1020Env* e, const uint8_t* unit, uint32_t ebx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4) {
    uint32_t L[23] = {0};
    int32_t t; uint32_t p; memcpy(&t, unit, 4); memcpy(&p, unit + 0x2c, 4);
    bool plr = t == 2 || (t > 3 && t <= 5);
    L[0] = 3;
    uint32_t a, b;
    if (plr) { memcpy(&a, (const uint8_t*)(uintptr_t)p + 0xc, 4); } else a = p ? e->ord10686(p) : 0;
    L[5] = a;
    if (plr) { memcpy(&b, (const uint8_t*)(uintptr_t)p + 0x10, 4); } else b = p ? e->ord10476(p) : 0;
    L[6] = b;
    L[1] = s0; L[4] = s1; L[11] = s3; L[12] = s4;
    if (ebx) { L[0] |= 4; L[10] = ebx; }
    if (s2 == 0xffffffffu) { e->a0db0(L, nullptr); return; }
    uint32_t C[23]; memcpy(C, L, sizeof C);
    C[4] = s2;
    e->a0db0(L, C);
}
}
