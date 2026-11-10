// 1.13c D2Game+0x5AFB0 (ACT5Q4_Callback_F0_Dispatch): ecx = quest node, plain ret, bool in AL.
// Written from the 1.13c disassembly as a rule over an environment E (HookEnv shape). Rule: node byte +0xC below 4 and byte +9 set:
// when +0xC is 0, 0x35EB0 is called (eax = 1, esi = node); the result is 1. Otherwise 0x33F30 (eax = node+4 word, ecx = node+0x10 word)
// finds a node; none -> 0. IsBadCodePtr(node+0xF0 word) nonzero is the fatal assert path (not modelled: E's code pointers are valid);
// then the found node's +0xF0 code pointer is called with ecx = found node and its AL is the result. E::callCode(fn, ecx) performs it.
#pragma once
#include <cstdint>

namespace d2g5afb0 {
template <class E> uint8_t Dispatch(E& e, uint32_t obj)
{
    const uint8_t st = e.rd8(obj + 0xC);
    if (st < 4 && e.rd8(obj + 9)) {
        if (st == 0) e.h(0x35EB0, 1, 0, 0, 0, obj);
        return 1;
    }
    const uint32_t ecx = e.rd32(obj + 0x10), eax = e.rd32(obj + 4);
    const uint32_t found = e.h(0x33F30, eax, ecx);
    if (!found) return 0;
    return (uint8_t)e.callCode(e.rd32(found + 0xF0), found);
}
}
