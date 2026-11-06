// 1.13c D2Game+0x6F620, injectable callees. EAX/ESI = the unit handed on (EAX is copied to ESI first), EBX and EDI ride through, stack
// word 0 = a pointer-sized argument (ret 4). 0x37540 runs with EAX = 0 and ESI = unit; a 124-byte stack block is filled by thunk 0xB132
// (5 stack words: &block, esi, w0, ebx, edi; ret 0x14) and read by thunk 0xB114 (4 stack words: esi, ebx, &block, edi; ret 0x10).
#include <cstdint>
struct G6f620Env {
    void (*f37540)(uint32_t eax, uint32_t esi);
    void (*b132)(const uint32_t* args);   // 5 stack words, in push-from-the-right order: args[0] is the first (&block)
    void (*b114)(const uint32_t* args);   // 4 stack words
};
// 1.13c D2Game+0x6F620
void g_6f620(uint32_t eax, uint32_t ebx, uint32_t edi, uint32_t w0, const G6f620Env& env)
{
    uint32_t block[31] = {};   // 124 bytes of residue in retail; both callees are seams, the pointer is what is logged
    const uint32_t esi = eax;
    env.f37540(0, esi);
    const uint32_t a132[5] = { (uint32_t)(uintptr_t)block, esi, w0, ebx, edi };
    env.b132(a132);
    const uint32_t a114[4] = { esi, ebx, (uint32_t)(uintptr_t)block, edi };
    env.b114(a114);
}
