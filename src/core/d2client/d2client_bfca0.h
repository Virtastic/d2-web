// Seams of D2Client 0xBFCA0 (the resistance line of an item tooltip). Imports and helpers are named by their retail address or ordinal.
#pragma once
#include <cstdint>
struct D2ClientBfca0Env {
    const uint16_t* (*str)(uint32_t id);                       // thunk 0xD1EC (D2Lang string by id)
    int32_t (*stat)(const void* unit, uint32_t stat);          // thunk 0xC1C6 (D2Common 10973, stat of a unit, layer 0)
    void (*colour)(uint32_t digit, uint16_t* s);               // 0xBF5F0 (colour prefix; digit pushed, text in EDI)
};
// 1.13c D2Client+0xBFCA0: stdcall(unit). out = the 256-wchar text global (D2Client+0x11C578), flag = the dword after it (+0x11C778).
void d2c_bfca0(const D2ClientBfca0Env& e, const uint8_t* unit, uint16_t* out, uint32_t* flag);
