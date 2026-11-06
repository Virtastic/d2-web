// 1.13c D2Game+0x68250 -- ClearFlag46ReturnTrue. Written from the 1.13c disassembly as a rule over the seam environment.
// ECX = game (passed through untouched), EDX = rec; the callee 0xDFCC0 is called with EAX = 1, EBX = D2Game+0x68150
// (the iterator callback, logged as the module token 0xB2000000|rva), ECX, one stack word 0. Afterwards the state block
// at rec+0x18 loses byte +0x46 and the result is 1 (AL; the upper bytes of EAX are the state pointer).
#include "../fn/hookenv.h"
namespace d2g {
uint32_t f68250(HookEnv& e, uint32_t ecx, uint32_t edx)
{
    e.h(0xDFCC0, 1, ecx, 0, 0xB2000000u | 0x68150u, 0, 0);
    e.wr8(e.rd32(edx + 0x18) + 0x46, 0);
    return 1;
}
}
