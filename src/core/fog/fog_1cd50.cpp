// 1.13c Fog+0x1CD50 (ordinal 10045): server-memory allocator wrapper, retail register layout with injectable callees.
// ECX = size/this word, EDX = second word, stack file, line (a third stack word is popped and unused: ret 0xc).
// The pool allocator Fog+0x1B560 gets EAX = EDX, ECX = ECX masked by the gate dword at Fog+0x2F350 (all ones when it is nonzero),
// EDX = ECX, stack file, line. A null block is reported through Fog+0x10F30 (channel 2, file, line, module string 0x268EC).
#include <cstdint>

struct Fog1CD50Env {
    uint32_t (*poolAlloc)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t file, uint32_t line);   // Fog+0x1B560
    void (*error)(uint32_t channel, uint32_t file, uint32_t line, uint32_t text);                     // Fog+0x10F30
    uint32_t errorText;                                                                               // module string 0x268EC
};

// 1.13c Fog+0x1CD50
extern "C" uint32_t fog_1cd50_regs(uint32_t ecx, uint32_t edx, uint32_t file, uint32_t line, uint32_t gate, const Fog1CD50Env* env) {
    const uint32_t mask = gate ? 0xFFFFFFFFu : 0u;
    const uint32_t p = env->poolAlloc(edx, ecx & mask, ecx, file, line);
    if (p == 0) env->error(2, file, line, env->errorText);
    return p;
}
