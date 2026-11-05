// 1.13c D2Game+0xDA930 and D2Game+0xDA970: EDI = amount, EBX = unit. amount < 1 returns 0.
// r = #10973(unit, stat, 0) - amount through thunk 0xA42A (stdcall 3); r < 0x100 (signed; DA930 also tests r < 0 first, the same set) gives 0;
// #10887(unit, stat, r, 0) through thunk 0xA472 (stdcall 4); returns r ? amount : 0. DA930 spends stat 10 (mana), DA970 stat 8 (life).
#pragma once
#include <cstdint>
namespace gda930 {
template <class E> uint32_t Spend(E& e, uint32_t amount, uint32_t unit, uint32_t stat)
{
    if ((int32_t)amount < 1) return 0;
    int32_t r = (int32_t)(e.h(0xA42A, 0, 0, 0, 0, 0, unit, stat, 0) - amount);
    if (r < 0x100) r = 0;
    e.h(0xA472, 0, 0, 0, 0, 0, unit, stat, (uint32_t)r, 0);
    return r ? amount : 0;
}
// 1.13c D2Game+0xDA930
template <class E> uint32_t Mana(E& e, uint32_t amount, uint32_t unit) { return Spend(e, amount, unit, 10); }
// 1.13c D2Game+0xDA970
template <class E> uint32_t Life(E& e, uint32_t amount, uint32_t unit) { return Spend(e, amount, unit, 8); }
}
