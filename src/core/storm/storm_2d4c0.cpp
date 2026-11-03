// Prior art used: none beyond the 1.13c decompile and disassembly.
#include <cstdint>
// 1.13c Storm+0x2D4C0 SBigAdd (ordinal 601), stdcall (dst, a, b): forwards onto the BigBuffer add 0x2D020, which takes
// dst in EAX and (a, b) on its stack.
extern "C" uint32_t Storm13c_2D4C0(uint32_t (*add2D020)(uint32_t eaxDst, uint32_t a, uint32_t b), uint32_t dst, uint32_t a, uint32_t b) {
    return add2D020(dst, a, b);
}
