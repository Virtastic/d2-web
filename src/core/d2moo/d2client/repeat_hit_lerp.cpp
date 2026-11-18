// Prior art used: UI interaction/rendering cross-checks.
// D2MOO has no D2Client module; local 1.13c cached decompiles define these rules.
#include <bit>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace d2::client::repeat_hit_lerp {
struct RepeatControl {
    std::byte prefix[0x20];
    std::uint32_t reset;
    std::byte gap[0x10];
    std::uint32_t heldA, heldB, unused, scale, startTick, lastTick, immediate;
};
static_assert(offsetof(RepeatControl, reset) == 0x20);
static_assert(offsetof(RepeatControl, heldA) == 0x34);
static_assert(offsetof(RepeatControl, scale) == 0x40);
static_assert(offsetof(RepeatControl, immediate) == 0x4c);

// Clock is an explicit platform input, with retail's unsigned tick wrap.
// 1.13c D2Client+0x3F060
std::uint32_t repeatStep(RepeatControl& control, std::uint32_t now) {
    if (control.heldA == 0 && control.heldB == 0) return 0;
    if (control.reset != 0) {
        control.heldA = control.heldB = control.reset = 0;
        return 1;
    }
    if (control.immediate != 0) {
        control.lastTick = now;
        return 1;
    }
    const auto elapsed = now - control.startTick;
    if (now - control.lastTick < 70) return 0;
    control.lastTick = now;
    if (elapsed > 4096) return (elapsed >> 11) * control.scale;
    if (elapsed > 3000) return control.scale >> 2;
    if (elapsed > 2000) return control.scale >> 4;
    if (elapsed > 1000) return control.scale >> 5;
    return 1;
}

// The source structure is packed: these three signed fields reside at
// 0x18075a, 0x18075e, 0x180762. The event's unsigned x/y are at 0xc/0xe.
// Native values avoid unaligned loads and retail pointer truncation.
// 1.13c D2Client+0x673A0
bool hitLine(std::int32_t left, std::int32_t baseline, std::int32_t line,
             std::uint16_t x, std::uint16_t y) {
    const auto signedBits = [](std::uint32_t v) { return std::bit_cast<std::int32_t>(v); };
    const auto right = signedBits(std::uint32_t(left) + 16u);
    const auto bottom = signedBits(std::uint32_t(baseline) + std::uint32_t(line) * 12u);
    const auto top = signedBits(std::uint32_t(bottom) - 12u);
    return left < x && x < right && top < y && y < bottom;
}

// VirtualAlloc receives the low 32 bits of dimension*dimension*steps+3.
// Native storage owns that allocation; checked stores keep retail allocation
// overrun inputs from corrupting host memory without imposing a dimension cap.
constexpr std::uint32_t lerpAllocationSize(std::uint32_t dimension, std::int32_t steps) {
    return dimension * dimension * std::uint32_t(steps) + 3u;
}

// 1.13c D2Client+0x8A3F0
std::vector<std::uint8_t> makeLerpTable(std::uint32_t dimension, std::int32_t steps) {
    std::vector<std::uint8_t> table(lerpAllocationSize(dimension, steps), 0);
    // Neither case reaches a store or a division in retail. Skipping the
    // otherwise empty row/column loops also handles arbitrarily large dimensions.
    if (dimension == 0 || steps == 0) return table;
    std::size_t output = 0;
    for (std::uint32_t row = 0; row < dimension; ++row) {
        for (std::uint32_t column = 0; column < dimension; ++column) {
            const auto numerator = std::bit_cast<std::int32_t>((column - row) << 16);
            const auto delta = std::int64_t(numerator) / steps;
            auto fixed = std::bit_cast<std::int32_t>(row << 16);
            // Retail decrements an x86 counter to zero, including signed steps.
            for (auto remaining = std::uint32_t(steps); remaining != 0; --remaining) {
                // x86 arithmetic shift with negative correction truncates to zero.
                table.at(output++) = static_cast<std::uint8_t>(fixed / 65536);
                fixed = std::bit_cast<std::int32_t>(std::uint32_t(fixed) + std::uint32_t(delta));
            }
        }
    }
    return table;
}
} // namespace d2::client::repeat_hit_lerp
