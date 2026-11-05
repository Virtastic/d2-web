// 1.13c D2Game+0xF21E0: build an 8-dword mode request on the stack and hand it to
// 75E10. Retail: EAX = mode, EDI = unit, stack (p1, p2, p3), ret 0xC. 75070 (called with ESI = the block) fatals on
// mode >= 0x10 (signed), calls #11103 (stack unit, 0), zeroes the block, stores mode at +0 and the unit at +4, and
// puts 0x65 at byte +0x15 when 74F20 (ECX = unit, EAX = mode) is nonzero, else 0x64. F21E0 then stores p2 at +0xC,
// p3 at +0x10 and calls 75E10 (EAX = block, stack p1, block, 1).
#include <cstdint>
#include <cstring>

struct D2GameF21E0Env {
    void (*o11103)(uint32_t unit, uint32_t v);                        // 0xA496
    uint32_t (*f74f20)(uint32_t ecxUnit, uint32_t eaxMode);           // 0x74F20
    uint32_t (*f75e10)(uint32_t p1, uint32_t* block, uint32_t one);   // 0x75E10 (EAX = block too)
    void (*fatal)(int32_t line);                                      // 75070 line 0x7B8
};

extern "C" uint32_t d2game_f21e0_impl(int32_t mode, uint32_t unit, uint32_t p1, uint32_t p2, uint32_t p3,
                                      const D2GameF21E0Env* e)
{
    uint32_t L[8];
    if (mode >= 0x10) { e->fatal(0x7B8); return 0; }
    e->o11103(unit, 0);
    std::memset(L, 0, sizeof L);
    L[0] = (uint32_t)mode;
    L[1] = unit;
    ((uint8_t*)L)[0x15] = e->f74f20(unit, (uint32_t)mode) ? 0x65 : 0x64;
    L[3] = p2;
    L[4] = p3;
    return e->f75e10(p1, L, 1);
}
