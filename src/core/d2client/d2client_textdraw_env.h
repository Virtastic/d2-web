// Seams for the D2Client centred-text rows (0x2D300, 0x2D3C0): D2Win set font (thunk 0xD3A8, ECX), text width (0xD35A,
// ECX = text), draw text (0xD37E, ECX = text, EDX = x, stack y, colour, 0), D2Lang language id (0xD1E6) and the module
// words 0x11B9A0 (x origin), 0x11B9A4 and 0xDBC4C (y origins).
#pragma once
#include <cstdint>
struct D2ClientTextDrawEnv {
    void (*font)(uint32_t id);
    int32_t (*width)(const uint16_t* text);
    void (*draw)(const uint16_t* text, int32_t x, int32_t y, uint32_t colour, uint32_t z);
    uint32_t (*lang)();
    int32_t x0, y0, y1;   // [0x11B9A0], [0x11B9A4], [0xDBC4C]
};
