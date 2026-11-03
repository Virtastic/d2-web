// Seams of the 1.13c D2Game+0xA3E10 port (src/core/d2game/d2game_a3e10.cpp); the FN probe fills them with recorders.
#pragma once
#include <cstdint>
struct D2GameA3E10Env {
    const char* assertFile;                         // DAT_6FD19FF9
    uintptr_t (*o10265)(uintptr_t line);            // Fog 10265 (assert message id)
    void (*o10024)(const char* file, uintptr_t msg);
    uintptr_t (*f21130)(uintptr_t unit);            // D2Game+0x1130: unit y
    uintptr_t (*f21100)(uintptr_t unit);            // D2Game+0x1100: unit x
    uintptr_t (*fB08F0)(uintptr_t unit, uintptr_t x, uintptr_t y);   // D2Game+0xB08F0: EAX = unit, stack x, y (ret 8)
};
extern "C" bool d2_game_A3E10(uintptr_t unit1, uintptr_t unit2, const D2GameA3E10Env* e);
