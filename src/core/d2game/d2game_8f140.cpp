// 1.13c D2Game+0x8F140 -- release the global item-cache slots flagged in use.
// No arguments. The globals are one block at RVA 0x1101F0: 17 flag bytes, then (+0x18) 17 records of 16 bytes whose dword 0 and
// dword 2 (+8) are heap blocks. Every flag equal to 1 is cleared and both blocks freed (0xA2EC, ECX = 0, EDX = block) and zeroed.
#include "d2game_8d6_common.h"
struct D2Game8F140Env {
    void (*freeBlock)(uint32_t block, uint32_t line);   // 0xA2EC (ECX = 0, EDX = block, stack: file 0x6FD1BAA0, line, 0)
};
// 1.13c D2Game+0x8F140
void d2game_port_8f140(uint32_t block, const D2Game8F140Env* env)
{
    for (uint32_t i = 0; i < 17; ++i) {
        if (d2g8::r8(block, i) != 1) continue;
        const uint32_t rec = 0x18 + i * 16;
        d2g8::w8(block, i, 0);
        if (d2g8::r32(block, rec)) { env->freeBlock(d2g8::r32(block, rec), 0x59D); d2g8::w32(block, rec, 0); }
        if (d2g8::r32(block, rec + 8)) { env->freeBlock(d2g8::r32(block, rec + 8), 0x5A2); d2g8::w32(block, rec + 8, 0); }
    }
}
