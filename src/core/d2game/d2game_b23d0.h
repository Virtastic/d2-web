// 1.13c D2Game+0xB23D0: EAX = unit; pd = #10920(unit) through thunk 0xA4C0 (stdcall 1); a null pd+0x34 is
// retail's fatal exit (error 0xDB, then _exit); otherwise the answer is (pd+0x34)->+4.
#pragma once
#include <cstdint>
#include <cstdlib>
namespace gb23d0 {
template <class E> uint32_t Rule(E& e, uint32_t unit)
{
    // 1.13c D2Game+0xB23D0
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t rec = e.rd32(pd + 0x34);
    if (!rec) abort();
    return e.rd32(rec + 4);
}
}
