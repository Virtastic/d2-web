// 1.13c Storm+0x41CC2: EH unwind funclet (EAX = [ebp-0x210], tail-jump to 0x4C20); ebp is the unwound frame (a 32-bit address in retail's layout).
#include <cstdint>
void Storm_Sub4C20(uint8_t* node);
void Storm_Unwind_41CC2(uint32_t ebp) {
    Storm_Sub4C20((uint8_t*)(uintptr_t)(*(const uint32_t*)(uintptr_t)(ebp - 0x210)));
}
