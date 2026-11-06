// Seams of D2Client 0x8E180 (item flag text). Imports and thunks are named by their retail address or ordinal.
#pragma once
#include <cstdint>
struct D2Client8e180Env {
    const uint16_t* (*str)(uint32_t id);                                         // thunk 0xD1EC (D2Lang string by id)
    int32_t (*itemFlag)(uint32_t item, uint32_t mask, uint32_t line, uint32_t file); // thunk 0xC1A8 (D2Common 10202, stdcall)
    uint32_t (*socketCount)(uint32_t item);                                      // thunk 0xC988 (D2Common 10366, low byte used)
};
// 1.13c D2Client+0x8E180: ESI = out buffer, stack = item; stdcall ret 4.
void d2c_8e180(const D2Client8e180Env& e, uint16_t* out, uint32_t item);
