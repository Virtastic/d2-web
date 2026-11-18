#include <bit>
#include <cstdint>
#include "d2gfx_blit.h"

struct D2GfxGouraudRows9050;
extern "C" void d2gfx_sub_8a40(const uint8_t*, uint8_t*, int32_t, int32_t,
                               int32_t, int32_t, int32_t, uint8_t, const D2GfxLitState*);

// 1.13c D2Gfx+0x095C0
// Prior art: D2MOO CmnSubtile.cpp's floor-tile Gouraud dispatch and interpolation.
// tile is the retail block header (format at +8); source is its +0x10 data
// pointer resolved by the caller, never a native pointer read from x86 bytes.
// light is retail EAX; destination is ECX. The raw seam matches sub_9050.
extern "C" void d2gfx_sub_95c0(
    uint8_t* destination, const uint8_t* tile, const uint8_t* source,
    const int32_t* light, const D2GfxLitState* lit,
    const D2GfxGouraudRows9050* rows,
    void (*drawRaw)(uint8_t*, const uint8_t*, int32_t, int32_t, int32_t,
                    int32_t, const D2GfxGouraudRows9050*)) {
    const int32_t aStep = std::bit_cast<int32_t>(uint32_t(light[3]) - uint32_t(light[0]));
    const int32_t bStep = std::bit_cast<int32_t>(uint32_t(light[2]) - uint32_t(light[1]));
    const int32_t a = std::bit_cast<int32_t>(uint32_t(light[0]) << 4);
    const int32_t b = std::bit_cast<int32_t>(uint32_t(light[1]) << 4);
    if (tile[8] & 4)
        d2gfx_sub_8a40(source, destination, a, aStep, b, bStep, 15, 7, lit);
    else
        drawRaw(destination, source, a, aStep, b, bStep, rows);
}
