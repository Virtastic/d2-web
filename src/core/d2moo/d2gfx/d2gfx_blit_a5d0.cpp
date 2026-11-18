#include <algorithm>
#include <bit>
#include <cstdint>
#include "d2gfx_blit_a5d0.h"

// Inputs that D2Gfx keeps in globals or renderer-owned row tables.  The retail
// routine receives the screen y in ECX, the edge array in EAX, and the
// remaining values on the stack; this explicit seam keeps those
// values testable without serializing retail pointers.
struct D2GfxRleBlitState {
    int32_t clipBottom, clipTop, clipLeft, clipRight, pitch;
    const uint8_t* lightLevels;
    const uint8_t* lightTable;
};
extern "C" void d2gfx_sub_9e90(const uint8_t*, int32_t, int32_t, uint8_t*, int32_t,
                               int32_t, int32_t, int32_t, int32_t, uint8_t,
                               const D2GfxRleBlitState*);
static int32_t d2gfx_wrap_add_a5d0(int32_t a, int32_t b) {
    return std::bit_cast<int32_t>(static_cast<uint32_t>(a) + static_cast<uint32_t>(b));
}
static int32_t d2gfx_wrap_sub_a5d0(int32_t a, int32_t b) {
    return std::bit_cast<int32_t>(static_cast<uint32_t>(a) - static_cast<uint32_t>(b));
}

// 1.13c D2Gfx+0x0A5D0
// Clipped iso Gouraud block. Edges are E0,E1,E2,E3; destinationY is the
// original hidden-this screen coordinate and destination is its output pointer.
extern "C" void d2gfx_sub_a5d0(uint8_t* destination,
                               const uint8_t* tile,
                               const uint8_t* source,
                               const int32_t* edges,
                               int32_t destinationX,
                               int32_t destinationY,
                               const D2GfxIsoBlitA5D0State* state) {
    if ((tile[8] & 4u) != 0) {
        const D2GfxRleBlitState rle{state->clipBottom, state->clipTop,
                                    state->clipLeft, state->clipRight,
                                    state->pitch, state->lightLevels,
                                    state->lightTable};
        const int32_t leftStep = d2gfx_wrap_sub_a5d0(edges[3], edges[0]);
        const int32_t rightStep = d2gfx_wrap_sub_a5d0(edges[2], edges[1]);
        d2gfx_sub_9e90(source, destinationX, destinationY, destination,
                       std::bit_cast<int32_t>(static_cast<uint32_t>(edges[0]) << 4),
                       leftStep,
                       std::bit_cast<int32_t>(static_cast<uint32_t>(edges[1]) << 4),
                       rightStep, 15, 7, &rle);
        return;
    }
    const int32_t first = std::clamp(d2gfx_wrap_sub_a5d0(state->clipTop, destinationY), 0, 15);
    const int32_t last = std::clamp(d2gfx_wrap_sub_a5d0(state->clipBottom, destinationY), 0, 15);
    int32_t left = std::bit_cast<int32_t>(static_cast<uint32_t>(edges[0]) << 4);
    int32_t right = std::bit_cast<int32_t>(static_cast<uint32_t>(edges[1]) << 4);
    const int32_t leftStep = d2gfx_wrap_sub_a5d0(edges[3], edges[0]);
    const int32_t rightStep = d2gfx_wrap_sub_a5d0(edges[2], edges[1]);
    uint8_t* outputRow = destination + static_cast<uint32_t>(state->pitch) * static_cast<uint32_t>(first);

    for (int32_t row = first; row < last; ++row) {
        const uint32_t skip = state->skips[row];
        const uint32_t width = state->widths[row];
        const int32_t screenX = d2gfx_wrap_add_a5d0(destinationX, static_cast<int32_t>(skip));
        int32_t begin = std::max(0, d2gfx_wrap_sub_a5d0(state->clipLeft, screenX));
        int32_t end = std::max(0, d2gfx_wrap_sub_a5d0(state->clipRight, screenX));
        end = std::min<int32_t>(static_cast<int32_t>(width), end);
        if (end > begin) {
            const int32_t levelLeft = left >> 7;
            const int32_t levelRight = right >> 7;
            const uint32_t levelBase =
                (static_cast<uint32_t>(levelLeft) * 32u + static_cast<uint32_t>(levelRight)) * 32u;
            const uint8_t* levels = state->lightLevels + levelBase + skip;
            const uint8_t* sourceRow = source + state->sourceRowOffsets[row] + static_cast<uint32_t>(begin);
            uint8_t* target = outputRow + skip + static_cast<uint32_t>(begin);
            for (int32_t col = begin; col < end; ++col) {
                const uint8_t level = levels[col - begin];
                *target++ = state->lightTable[static_cast<uint32_t>(level) * 256u + sourceRow[col - begin]];
            }
        }
        outputRow += state->pitch;
        left = d2gfx_wrap_add_a5d0(left, leftStep);
        right = d2gfx_wrap_add_a5d0(right, rightStep);
    }
}
