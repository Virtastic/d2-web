// 1.13c D2Client+0x340F0 -- fastcall (ECX this, EDX p, stack a0..a4, ret 0x14) text-line builder.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
void f340f0(GE& e, uint32_t self, uint32_t p, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
{
    if (!p || !a0) return;
    uint16_t idw; __builtin_memcpy(&idw, (const void*)(uintptr_t)a0, 2);
    const uint32_t id = (uint32_t)(int32_t)(int16_t)idw;
    const uint32_t rec = e.h(0x17F0, id);
    if (!rec) return;
    const uint32_t r1 = e.h(0xCEE0, 0, 0, 0, 0, 0, self, e.rd32(rec + 0x18), id, a1);
    const uint32_t r2 = e.h(0xCEE0, 0, 0, 0, 0, 0, self, e.rd32(rec + 0x1c), id, a1);
    uint32_t s14 = 0, s18 = 0, s1c = 0;
    e.h(0x33240, 0, self, e.stk(&s1c), 0, 0, e.stk(&s18), e.stk(&s14), r1, r2, p, 0x80, 0);
    uint32_t s0c = 0, s8 = 0, s10 = 0;
    e.h(0x2E190, a0, 0, 0, a1, 0, self, e.stk(&s0c), e.stk(&s8), e.stk(&s10));
    if (!s1c && !s18) { e.h(0x2F0A0, s8, s0c, 0, 0, 0, s10, a2, a3, a4); return; }
    if (!s0c && !s8) { e.h(0x2F0A0, s18, s1c, 0, 0, 0, s14, a2, a3, a4); return; }
    char16_t b1[32] = {}, b2[32] = {};
    e.h(0x2E140, s18, 0, e.stk(b1), 0, 0, s1c);
    e.h(0x2E140, s8, 0, e.stk(b2), 0, 0, s0c);
    e.h(0x2D3C0, 0, 0, 0, 0, 0, e.stk(b1), e.stk(b2), a2, a3, a4, s14, s10);
}
}
