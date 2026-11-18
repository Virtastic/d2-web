#include <bit>
#include <cstdint>

struct D2GfxGouraudRows9050 {
    int32_t destinationOffsets[15];
    uint32_t rowWidths[15];
    int32_t sourceOffsets[15];
    const uint8_t* levelIndices;
    const uint8_t* lightLevelVariations;
    uint32_t framebufferStride;
};

static_assert(sizeof(D2GfxGouraudRows9050::destinationOffsets) == 60);
static_assert(sizeof(D2GfxGouraudRows9050::rowWidths) == 60);
static_assert(sizeof(D2GfxGouraudRows9050::sourceOffsets) == 60);

static int32_t d2gfx_add_wrap_9050(int32_t value, int32_t increment) {
    return std::bit_cast<int32_t>(static_cast<uint32_t>(value) + static_cast<uint32_t>(increment));
}

// 1.13c D2Gfx+0x09050
// Direct row-table form of the retail iso Gouraud drawer. D2Gfx owns the tables;
// this entry point receives their data explicitly so its x86 indexing is testable.
extern "C" void d2gfx_sub_9050(uint8_t* destination,
                               const uint8_t* source,
                               int32_t x,
                               int32_t xStep,
                               int32_t y,
                               int32_t yStep,
                               const D2GfxGouraudRows9050* rows) {
    uint32_t destinationRow = 0;
    for (uint32_t row = 0; row < 15; ++row) {
        const uint32_t width = rows->rowWidths[row];
        // Retail's switch falls through in four-pixel groups. Other widths write nothing.
        if (width == 4 || width == 8 || width == 12 || width == 16 || width == 20 ||
            width == 24 || width == 28 || width == 32) {
            const uint32_t levelBase =
                (static_cast<uint32_t>(x >> 7) * 32u + static_cast<uint32_t>(y >> 7)) * 32u;
            const uint32_t destinationOffset = static_cast<uint32_t>(rows->destinationOffsets[row]);
            const uint32_t sourceOffset = static_cast<uint32_t>(rows->sourceOffsets[row]);
            for (uint32_t column = 0; column < width; ++column) {
                const uint8_t level = rows->levelIndices[levelBase + destinationOffset + column];
                const uint8_t texel = source[sourceOffset + column];
                destination[destinationRow + destinationOffset + column] =
                    rows->lightLevelVariations[static_cast<uint32_t>(level) * 256u + texel];
            }
        }
        destinationRow += rows->framebufferStride;
        x = d2gfx_add_wrap_9050(x, xStep);
        y = d2gfx_add_wrap_9050(y, yStep);
    }
}
