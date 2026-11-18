// Prior art used: none beyond the 1.13c decompile and disassembly.
#include <cstdint>
// 1.13c Storm+0x2D370 SBigShl (ordinal 633), stdcall (dst, src, shift): forwards onto the BigBuffer shift-left 0x2C210,
// which takes dst in ECX, the shift count in EAX and src on its stack.
extern "C" uint32_t Storm13c_2D370(uint32_t (*shl2C210)(uint32_t ecxDst, uint32_t eaxShift, uint32_t src), uint32_t dst, uint32_t src, uint32_t shift) {
    return shl2C210(dst, shift, src);
}
