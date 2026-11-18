// Prior art used: none beyond the 1.13c decompile and disassembly.
#include <cstdint>
// 1.13c Storm+0x2D440 SBigFromUnsigned (ordinal 612), stdcall (dst, value): forwards onto 0x2B6A0 (value vector := one
// dword), which takes dst in EAX and the value on its stack.
extern "C" uint32_t Storm13c_2D440(uint32_t (*set2B6A0)(uint32_t eaxDst, uint32_t value), uint32_t dst, uint32_t value) {
    return set2B6A0(dst, value);
}
