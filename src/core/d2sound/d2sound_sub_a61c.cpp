// 1.13c D2sound+0x0A61C -- MSVC CRT __adj_fdivr_m32 (x87 fdivr fixup; caller 0x8B50).
// x87 state is a host seam (no float): fdivrp is invoked through the callback.
#include <cstdint>

// Returns 0 if the operand is Inf/NaN (exponent all ones: no divide), else 1 after calling fdivrp.
// (retail's two fdivrp branches, on FPU status C-bits, are the same operation; EAX passes through.)
int D2sound_A61C(uint32_t operand, void (*fdivrp_sti_st)()) {
    if ((operand & 0x7f800000u) == 0x7f800000u) return 0;
    fdivrp_sti_st();
    return 1;
}
