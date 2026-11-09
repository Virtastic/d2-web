// 1.13c Storm+0x41A72: EH unwind funclet (eh_vector_destructor_iterator([ebp+4] + 0x160, 12, 4, 0x53C0), back to front); ebp is the unwound frame (a 32-bit address in retail's layout).
#include <cstdint>
void Storm_Sub53C0(char* e);
void Storm_Unwind_41A72(uint32_t ebp) {
    for (int i = 3; i >= 0; --i) Storm_Sub53C0((char*)(uintptr_t)(*(const uint32_t*)(uintptr_t)(ebp + 4) + 0x160 + 12 * i));
}
