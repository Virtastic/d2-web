// 1.13c D2Game: the inlined unit coordinate reads shared by the skill start callbacks at 0xB3120 and 0xB3930.
// Objects, items and tiles (types 2, 4, 5) hold their coordinates in the static path at +0x2C (x +0x0C, y
// +0x10). Every other type asks the dynamic path at +0x2C (x: 0xA454, y: 0xA3B8, one stack word each); no path: 0.
#pragma once
#include <cstdint>

template <class E> bool D2GameUnitStaticPath(E& e, uint32_t unit)
{
    const int32_t type = (int32_t)e.rd32(unit);
    return type == 2 || (type > 3 && type <= 5);
}
template <class E> uint32_t D2GameUnitX(E& e, uint32_t unit)
{
    const uint32_t path = e.rd32(unit + 0x2C);
    if (D2GameUnitStaticPath(e, unit)) return e.rd32(path + 0xC);
    return path ? e.h(0xA454, 0, 0, 0, 0, 0, path) : 0;
}
template <class E> uint32_t D2GameUnitY(E& e, uint32_t unit)
{
    const uint32_t path = e.rd32(unit + 0x2C);
    if (D2GameUnitStaticPath(e, unit)) return e.rd32(path + 0x10);
    return path ? e.h(0xA3B8, 0, 0, 0, 0, 0, path) : 0;
}
