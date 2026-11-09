// 1.13c Storm+0x419B8: EH unwind funclet (push [ebp-0x10], call 0x4B80); ebp is the unwound frame (a 32-bit address in retail's layout).
#include <cstdint>
void sub_4B80(void* p);
void Storm_Unwind_419B8(uint32_t ebp) {
    sub_4B80((void*)(uintptr_t)(*(const uint32_t*)(uintptr_t)(ebp - 0x10)));
}
