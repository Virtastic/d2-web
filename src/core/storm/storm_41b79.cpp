// 1.13c Storm+0x41B79: EH unwind funclet (push [ebp+4] + 0x3050, call 0x4520); ebp is the unwound frame (a 32-bit address in retail's layout).
#include <cstdint>
void Storm_Sub4520_Own(uint8_t* p);
void Storm_Unwind_41B79(uint32_t ebp) {
    Storm_Sub4520_Own((uint8_t*)(uintptr_t)(*(const uint32_t*)(uintptr_t)(ebp + 4) + 0x3050));
}
