#pragma once
#include <cstdint>
// The D2Gfx globals sub_99C0 / sub_ABF0 read (retail 0x6FA9xxxx), gathered; the replay probe fills it from the vector.
struct D2GfxBlitState {
    int32_t yTop, yBot, xLeft, xRight;            // outer clip: 0x6FA9D45C, 0x6FA948B8, 0x6FA948C0, 0x6FA9D454
    int32_t inY0, inY1, inX0, inX1;               // inner rect (start inside = no per-run clip): 0x6FA95448, 0x6FA948BC, 0x6FA9D458, 0x6FA948B0
    int32_t c2Bottom, c2Top, c2Left, c2Right;     // per-run clip: 0x6FA948CC, 0x6FA9D464, 0x6FA95444, 0x6FA9D46C
    uint8_t* base; int32_t pitch;                 // surface base 0x6FA94778, pitch 0x6FA9477C
    const uint8_t* shade[3];                      // *0x6FA948A8 = {sel>=0xC0, >=0x80, >=0x40}
    const int32_t* rowOff;                        // 0x6FA94950: per-row byte offset
};
// The globals sub_8A40 / sub_9E90 read: per-run clip (0x6FA948CC, 0x6FA9D464, 0x6FA95444, 0x6FA9D46C), pitch 0x6FA9477C,
// the light map 0x6FA95450 (32x32 cells of 32 bytes) and the 256x256 shade table 0x6FA91678.
struct D2GfxLitState { int32_t c2Bottom, c2Top, c2Left, c2Right; int32_t pitch; const uint8_t* lmap; const uint8_t* tab; };
