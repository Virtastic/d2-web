// 1.13c Storm+0x41C8C: EH unwind funclet (push ebp - 0x12c, call 0x1250); ebp is the unwound frame (a 32-bit address in retail's layout).
#include <cstdint>
extern "C" void storm_port_1250(uint32_t* r);   // the exported entry; Storm_Sub1250 itself is file-local
void Storm_Unwind_41C8C(uint32_t ebp) {
    uint32_t r[16] = { ebp - 0x12c };
    storm_port_1250(r);
}
