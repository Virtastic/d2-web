// 1.13c D2Game+0xB6CE0 -- ECX = unit, EDX = slot holding a unit, one stack argument = counter. A non-null unit that is not the slot's unit and whose +0x10 field is neither 0 nor 12 increments the counter (EAX then holds the counter's address); otherwise EAX is not touched.
#include <cstdint>
uint32_t d2game_b6ce0(uint32_t eax, const uint32_t* unit, const uint32_t* slot, uint32_t* counter) {
    if ((uint32_t)(uintptr_t)unit == *slot || !unit) return eax;
    const uint32_t mode = unit[4];
    if (mode == 0 || mode == 12) return eax;
    ++*counter;
    return (uint32_t)(uintptr_t)counter;
}
