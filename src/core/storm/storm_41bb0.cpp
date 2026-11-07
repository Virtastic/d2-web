// 1.13c Storm+0x41BB0: EH unwind funclet (eh_vector_destructor_iterator([ebp+4] + 8, 0x18, 0x203, 0x47D0), back to front); ebp is the unwound frame (a 32-bit address in retail's layout).
#include <cstdint>
void Storm_Sub47D0(intptr_t* e);
void Storm_Unwind_41BB0(uint32_t ebp) {
    for (int i = 0x202; i >= 0; --i) Storm_Sub47D0((intptr_t*)(uintptr_t)(*(const uint32_t*)(uintptr_t)(ebp + 4) + 8 + 0x18 * i));
}
