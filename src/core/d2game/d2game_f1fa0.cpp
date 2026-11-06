// 1.13c D2Game+0xF1FA0, injectable callees. Registers: EAX = pGame (stack word 0 of 0x75070), ECX = unit; stack words 0..2 (ret 0xC):
// word 0 = first argument of 0x75E10, word 1 goes to block+0xC, word 2 to block+0x10. The 0x20-byte stack block is set up by 0x75070
// (ESI = block, stack word 0 = pGame, ret 4); 0x75E10 takes (word0, &block, 1), ret 0xC; its result stays in EAX. 0xAD18 (D2Common 11005): 2 words.
#include <cstdint>
struct GF1FA0Env {
    uint32_t (*f75070)(uint32_t esi, uint32_t s0);
    uint32_t (*o11005)(uint32_t s0, uint32_t s1);
    uint32_t (*f75e10)(uint32_t s0, uint32_t s1, uint32_t s2);
};
// 1.13c D2Game+0xF1FA0
uint32_t g_f1fa0(uint32_t eax, uint32_t ecx, uint32_t w0, uint32_t w1, uint32_t w2, const GF1FA0Env& env)
{
    uint32_t blk[8];
    env.f75070((uint32_t)(uintptr_t)blk, eax);
    env.o11005(*(uint32_t*)(uintptr_t)(ecx + 0x2C), 0);
    blk[3] = w1;
    blk[4] = w2;
    return env.f75e10(w0, (uint32_t)(uintptr_t)blk, 1);
}
