// 1.13c Storm+0x419DB: EH unwind funclet (EAX = [ebp+4] + 0xc, tail-jump to 0x4C20); ebp is the unwound frame (a 32-bit address in retail's layout).
#include <cstdint>
void Storm_Sub4C20(uint8_t* e);
void Storm_Unwind_419DB(uint32_t ebp) {
    Storm_Sub4C20((uint8_t*)(uintptr_t)(*(const uint32_t*)(uintptr_t)(ebp + 4) + 0xc));
}
