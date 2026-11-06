// Seams of the D2Launch 0x12560 port (src/core/d2launch/d2launch_n57.cpp): the D2Win/D2Lang thunks with retail's registers.
#pragma once
#include <cstdint>
struct D2l12560 {
    uint32_t (*o10013)();                            // 0x97C6: the host string
    void (*o10106)(uint32_t ecx);                    // 0x996A
    uint32_t (*o10003)(uint32_t id);                 // 0x9B32 (ECX = string id)
    void (*o10020)(uint32_t ecx, uint32_t edx);      // 0x98C8
    void (*o10167)(uint32_t ecx, uint32_t edx);      // 0x9976
    uint32_t (*o10147)(uint32_t ecx, uint32_t edx);  // 0x9862
    uint32_t g54f8, g5514, g5510, g52a0;             // DAT_6fa654f8, 65514, 65510, 652a0
};
uint32_t d2launch_sub_12560(const D2l12560* s);
