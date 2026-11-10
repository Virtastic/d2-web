// Host test: the light map's per-cell colour blend.
#include "../d2client/light_cell_blend.h"
#include <cassert>
#include <cstdio>

using d2::client::LightCell;
using d2::client::lightCellBlend;

int main() {
    // A dark cell takes the new light's colour, less the reciprocal's rounding (0x10000/100 = 655).
    LightCell c = lightCellBlend({ 0, 0, 0, 0 }, 100, 255, 128, 0);
    assert(c.level == 100 && c.r == 254 && c.g == 127 && c.b == 0);
    // A power-of-two level divides exactly.
    c = lightCellBlend({ 0, 0, 0, 0 }, 64, 200, 100, 50);
    assert(c.level == 64 && c.r == 200 && c.g == 100 && c.b == 50);
    // Two lights average by weight: 64 of white ambient and 64 of red.
    c = lightCellBlend({ 64, 255, 255, 255 }, 64, 255, 0, 0);
    assert(c.level == 128 && c.r == 255 && c.g == 127 && c.b == 127);
    // The level stops at 255 but the weighted sum does not: a full cell given more light saturates.
    c = lightCellBlend({ 255, 200, 100, 0 }, 255, 200, 100, 0);
    assert(c.level == 255 && c.r == 255 && c.g == ((100u * 255u * 2u) * 257u >> 16) && c.b == 0);
    // A white player light over a coloured ambient pulls the cell toward white.
    c = lightCellBlend({ 40, 255, 64, 48 }, 215, 255, 255, 255);
    assert(c.level == 255 && c.r == 254);   // 255 * 255 * 257 / 65536 falls just short of 255
    assert(c.g == ((64u * 40u + 255u * 215u) * 257u >> 16) && c.b == ((48u * 40u + 255u * 215u) * 257u >> 16));
    static_assert(lightCellBlend({ 10, 9, 9, 9 }, 0, 255, 255, 255).level == 10);
    std::puts("client_light_blend: ok");
    return 0;
}
