#pragma once
#include <cstdint>

struct D2GfxIsoBlitA5D0State {
    int32_t clipTop;
    int32_t clipBottom;
    int32_t clipLeft;
    int32_t clipRight;
    int32_t pitch;
    const uint8_t* lightLevels;
    const uint8_t* lightTable;
    const uint32_t* sourceRowOffsets;
    const uint32_t* skips;
    const uint32_t* widths;
};

extern "C" void d2gfx_sub_a5d0(uint8_t* destination,
                               const uint8_t* tile,
                               const uint8_t* source,
                               const int32_t* edges,
                               int32_t destinationX,
                               int32_t destinationY,
                               const D2GfxIsoBlitA5D0State* state);
