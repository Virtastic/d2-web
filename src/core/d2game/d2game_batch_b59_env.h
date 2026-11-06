#pragma once
#include <cstdint>

struct D2GameBatchB59Env {
    uint32_t (*ord)(uint32_t n, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e);
    uint32_t (*subDFA90)(void* unit);                       // 0xDFA90
    uint32_t (*subE0A60)(void* a, void* b);                 // 0xE0A60
    uint32_t (*sub218E0)();                                 // 0x18E0 (record, +8 flags)
    uint32_t (*sub21130)(void* unit);                       // 0x1130
    uint32_t (*sub21100)(void* unit);                       // 0x1100
    int32_t  (*subB0A20)(void* unit);                       // 0xB0A20
    uint32_t (*subB08A0)(void* unit);                       // 0xB08A0 (byte result)
    void     (*sub354F0)(void* a, void* b);                 // 0x354F0
    void     (*sub54130)(uint32_t a, void* b, void* c, uint32_t d); // 0x34130
    uint32_t (*sub21280)();                                 // 0x1280
};
