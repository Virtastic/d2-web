// 1.13c D2Game+0x69D60: object mode callback "reset shrine" (table entry). edx = the object, no stack words. When
// the reset test (0xB186: object) is zero nothing happens and the result is 0. Otherwise the object is refreshed
// (0xA4DE: object), gets flag bit 0 at +0xC4, goes to mode 0 (0xA4A2: object, 0) and the word at +0x0C of its
// data block (+0x14) is cleared; that data block pointer is the result.
#pragma once
#include <cstdint>

namespace g69d60 {
template <class E> uint32_t Reset(E& e, uint32_t obj)
{
    if (!e.h(0xB186, 0, 0, 0, 0, 0, obj)) return 0;
    e.h(0xA4DE, 0, 0, 0, 0, 0, obj);
    if (obj) e.wr32(obj + 0xC4, e.rd32(obj + 0xC4) | 1);
    e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 0);
    const uint32_t data = e.rd32(obj + 0x14);
    e.wr32(data + 0xC, 0);
    return data;
}
}
