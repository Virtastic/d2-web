// 1.13c Storm+0x41AA2: EH unwind funclet (ECX = [ebp+4] + 0x1c0, tail-jump to 0x53C0); ebp is the unwound frame (a 32-bit address in retail's layout).
#include <cstdint>
void Storm_Sub53C0(char* e);
void Storm_Unwind_41AA2(uint32_t ebp) {
    Storm_Sub53C0((char*)(uintptr_t)(*(const uint32_t*)(uintptr_t)(ebp + 4) + 0x1c0));
}
