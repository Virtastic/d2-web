// Prior art used: none beyond the 1.13c decompile and disassembly.
#include <cstdint>
// 1.13c Storm+0x2D400 SBigMul (ordinal 622), stdcall (dst, a, b): forwards onto the BigBuffer multiply 0x2C840 with
// stack (dst, a, b, dst+0x14): the fourth word is dst's scratch stack (the 16 temporaries at +0x14).
extern "C" uint32_t Storm13c_2D400(uint32_t (*mul2C840)(uint32_t dst, uint32_t a, uint32_t b, uint32_t scratch), uint32_t dst, uint32_t a, uint32_t b) {
    return mul2C840(dst, a, b, dst + 0x14);
}
