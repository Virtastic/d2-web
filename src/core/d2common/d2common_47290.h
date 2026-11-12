// 1.13c D2Common+0x47290: item property callback "ethereal" (table entry). Nine stack words (ret 0x24): the item,
// an apply flag, seven unread. A zero flag, a null unit, a unit that is not an item (type 4 at +0x00) or an item
// whose data (+0x14) already carries flag 0x400000 (+0x18) gives 0. So does an item without durability (0x26D40:
// item). Otherwise the item-type check runs (0x46A40: item) and the result is 1.
#pragma once
#include <cstdint>

namespace c47290 {
template <class E> uint32_t Apply(E& e, uint32_t item, uint32_t flag)
{
    if (!flag || !item || e.rd32(item) != 4) return 0;
    const uint32_t data = e.rd32(item + 0x14);
    if (data && (e.rd32(data + 0x18) & 0x400000)) return 0;
    if (!e.h(0x26D40, 0, 0, 0, 0, 0, item)) return 0;
    e.h(0x46A40, 0, 0, 0, 0, 0, item);
    return 1;
}
}
