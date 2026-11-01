// Seams for the D2Client string-building rows (0x2C950, 0x2C9F0, 0x2CB60): the D2Lang string table (D2Client thunk 0xD1EC,
// ECX = id), the D2Lang Unicode sprintf / strcat imports and the D2Win text popup (thunk 0xD384). The capture runs the real
// imports; a probe supplies recorders that write the hook records the capture writes.
#pragma once
#include <cstdint>
struct D2ClientUStrEnv {
    const uint16_t* (*str)(uint32_t id);
    void (*sprintf)(uint32_t cap, uint16_t* dst, const uint16_t* fmt, int32_t v);
    void (*strcat)(uint16_t* dst, const uint16_t* src);
    void (*popup)(const uint16_t* buf, uint32_t edx, uint32_t x, uint32_t y, uint32_t z);
    uint32_t g48, g4c;   // module words D2Client+0xDBC48 / +0xDBC4C
};
