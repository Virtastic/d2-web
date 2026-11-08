// 1.13c D2Game+0x98E30 -- a skill-effect trampoline (ECX = skill-call record, EAX = register word, stack = a word handed on in EAX; ret 4).
// Written from the 1.13c disassembly as a rule over E. Builds a six-word block {0, -1, 0, EAX, EAX, 0} and calls the launcher 0xA0C70 with ECX = the record,
// EAX = the stack word, stack = (record, 0, 0, 3, 0xA783, callback 0x6FCBED70, &block, 0). The launcher fills the block. The result is block[0], or
// block[2] when block[0] is zero (the previous decompile lost the block and returned a constant 0).
#ifndef D2GAME_RULE_98E30
#define D2GAME_RULE_98E30
#include <cstdint>

namespace g98e30 {
template <class E> uint32_t Trampoline(E& e, uint32_t eaxIn, uint32_t rec, uint32_t stackWord)
{
    uint32_t blk[6] = { 0, 0xFFFFFFFFu, 0, eaxIn, eaxIn, 0 };
    const uint32_t pre[6] = { blk[0], blk[1], blk[2], blk[3], blk[4], blk[5] };   // the capture logs the block as it was at hook entry
    e.h(0xA0C70, stackWord, 0, 0, 0, 0, rec, 0, 0, 3, 0xA783, 0xB2000000u | 0xBED70, e.stk(blk));
    e.blk(0xA0C70, pre, 6);
    return blk[0] ? blk[0] : blk[2];
}
}
#endif
