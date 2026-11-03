// Prior art used: none beyond the 1.13c decompile and disassembly.
#include <cstdint>
// 1.13c Storm+0x2D310 SBigSub (ordinal 636), stdcall (dst, a, b): a pure forwarder onto the BigBuffer subtract 0x2BA20,
// which takes dst in EAX and (a, b) on its stack. Words are retail 32-bit addresses; eax of the callee is the result.
extern "C" uint32_t Storm13c_2D310(uint32_t (*sub2BA20)(uint32_t eaxDst, uint32_t a, uint32_t b), uint32_t dst, uint32_t a, uint32_t b) {
    return sub2BA20(dst, a, b);
}
