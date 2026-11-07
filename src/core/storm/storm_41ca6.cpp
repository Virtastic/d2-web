// 1.13c Storm+0x41CA6: EH unwind funclet (push ebp - 0x12c, call 0x11A0); ebp is the unwound frame (a 32-bit address in retail's layout).
#include <cstdint>
extern "C" void storm_port_11a0(uint32_t* r);   // the exported entry; Storm_Sub11A0 itself is file-local
void Storm_Unwind_41CA6(uint32_t ebp) {
    uint32_t r[16] = { ebp - 0x12c };
    storm_port_11a0(r);
}
