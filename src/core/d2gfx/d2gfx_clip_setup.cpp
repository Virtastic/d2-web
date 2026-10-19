#include <cstdint>

// 1.13c D2Gfx+0x08970
// Scalar representation of the twenty renderer globals, in the spec's address order.
// Explicit uint32_t subtraction preserves retail's wrapping x86 arithmetic.
extern "C" void d2gfx_sub_8970(int32_t width, int32_t height, uint32_t selector, uint32_t* out) {
    uint32_t left = 0;
    if (selector == 1) width -= width / 2;
    else if (selector == 2) left = static_cast<uint32_t>(width / 2);
    const uint32_t right = static_cast<uint32_t>(width);
    const uint32_t bottom = static_cast<uint32_t>(height);
    out[0] = left;
    out[1] = bottom - 47;
    out[2] = bottom - 63;
    out[3] = left - 32;
    out[4] = right;
    out[5] = left - 32;
    out[6] = right;
    out[7] = right;
    out[8] = bottom - 47;
    out[9] = bottom - 47;
    out[10] = uint32_t(-16);
    out[11] = 0;
    out[12] = right - 32;
    out[13] = uint32_t(-32);
    out[14] = 0;
    out[15] = bottom - 79;
    out[16] = left;
    out[17] = right - 32;
    out[18] = 0;
    out[19] = left;
}
