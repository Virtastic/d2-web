#include <cstdint>
#include <cstddef>

// Explicit replacements for the renderer globals and the incoming ESI palette.
struct D2GfxRleClip {
    int32_t stride; // D2Gfx+0x1477C
    int32_t top;    // D2Gfx+0x1D464
    int32_t bottom; // D2Gfx+0x148CC
    int32_t left;   // D2Gfx+0x15444
    int32_t right;  // D2Gfx+0x1D46C
    const uint8_t* palette;
};

// 1.13c D2Gfx+0x0A310
void d2gfx_rle_clipped(const uint8_t* source, int32_t x, int32_t y,
                       uint8_t* destination, int32_t lines, const D2GfxRleClip& clip) {
    int32_t completed = 0;
    int32_t offset = 0;
    while (completed < lines) {
        const uint32_t skip = *source++;
        const uint32_t length = *source++;
        if (skip == 0 && length == 0) {
            ++completed;
            y = static_cast<int32_t>(static_cast<uint32_t>(y) + 1u);
            destination += clip.stride;
            offset = 0;
            continue;
        }
        if (y >= clip.bottom) return;
        offset = static_cast<int32_t>(static_cast<uint32_t>(offset) + skip);
        if (y >= clip.top) {
            const uint32_t run_x = static_cast<uint32_t>(x) + static_cast<uint32_t>(offset);
            int32_t first = static_cast<int32_t>(static_cast<uint32_t>(clip.left) - run_x);
            int32_t end = static_cast<int32_t>(static_cast<uint32_t>(clip.right) - run_x);
            if (first < 1) first = 0;
            if (end < 1) end = 0;
            if (end > static_cast<int32_t>(length)) end = static_cast<int32_t>(length);
            const int32_t count = static_cast<int32_t>(static_cast<uint32_t>(end) - static_cast<uint32_t>(first));
            // Retail's fall-through jump table has cases 1..32 and no default writes.
            if (count > 0 && count <= 32) {
                for (int32_t i = count; i-- > 0;)
                    destination[static_cast<ptrdiff_t>(offset) + first + i] = clip.palette[source[first + i]];
            }
        }
        source += length;
        offset = static_cast<int32_t>(static_cast<uint32_t>(offset) + length);
    }
}
