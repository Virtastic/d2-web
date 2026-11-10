#pragma once

#include <cstdint>

namespace d2::client {

// One cell of the light map: how bright it is and what colour its light has.
struct LightCell { uint8_t level, r, g, b; };

// Adding light to a cell. The level rises by the amount and stops at 255. Each colour channel becomes the
// average of the cell's colour and the new light's, weighted by the cell's old level and the amount, divided
// by the new (capped) level through a 16.16 reciprocal, and stops at 255. So a cell at level 0 takes the new
// light's colour outright, and light added to a cell already full pushes every channel it carries up.
constexpr LightCell lightCellBlend(LightCell cell, int amount, uint8_t r, uint8_t g, uint8_t b) {
    int level = cell.level + amount;
    if (level >= 255) level = 255;
    const uint32_t reciprocal = level > 0 ? 0x10000u / static_cast<uint32_t>(level) : 0u;
    const auto channel = [&](uint8_t was, uint8_t add) {
        const uint32_t v = ((static_cast<uint32_t>(was) * cell.level + static_cast<uint32_t>(add) * static_cast<uint32_t>(amount)) * reciprocal) >> 16;
        return static_cast<uint8_t>(v > 255 ? 255 : v);
    };
    return { static_cast<uint8_t>(level), channel(cell.r, r), channel(cell.g, g), channel(cell.b, b) };
}

} // namespace d2::client
