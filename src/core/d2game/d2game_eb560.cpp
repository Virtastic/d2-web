// 1.13c D2Game+0xEB560, injectable callees. ECX = unit (the D2Common thunk 0xA4C0's argument, 1 word); stack word 0 = pointer to the
// read cursor, word 1 = end of the buffer; ret 8. The record 0xA4C0 returns holds three destination pointers at +0x10; the thunk
// 0xA3FA (4 words) copies 0x60 bytes of the section to each, flag 1. Failure returns 0xF, success 0 with the cursor moved on by 0x12A.
#include <cstdint>
struct GEB560Env {
    uint32_t (*a4c0)(uint32_t unit);
    uint32_t (*a3fa)(uint32_t dst, uint32_t src, uint32_t len, uint32_t flag);
};
// 1.13c D2Game+0xEB560
uint32_t g_eb560(uint32_t ecx, uint32_t cursorPtr, uint32_t end, const GEB560Env& env)
{
    const uint32_t cur = *(uint32_t*)(uintptr_t)cursorPtr;
    if (cur + 0x12A > end) return 0xF;
    if (*(uint32_t*)(uintptr_t)cur != 0x216F6F57u || *(uint32_t*)(uintptr_t)(cur + 4) != 6) return 0xF;
    const uint32_t rec = env.a4c0(ecx);
    uint32_t src = cur + 0xA;
    for (uint32_t i = 0; i < 3; ++i, src += 0x60)
        env.a3fa(*(uint32_t*)(uintptr_t)(rec + 0x10 + 4 * i), src, 0x60, 1);
    *(uint32_t*)(uintptr_t)cursorPtr = cur + 0x12A;
    return 0;
}
