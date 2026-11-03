// Prior art used: none beyond the 1.13c decompile and disassembly.
#include <cstdint>
// 1.13c Storm+0x2D460 SBigFromBinary (ordinal 609), stdcall (dst, bytes, count): forwards onto 0x2B760 (value vector
// := little-endian bytes), which takes dst in EAX and (bytes, count) on its stack.
extern "C" uint32_t Storm13c_2D460(uint32_t (*from2B760)(uint32_t eaxDst, uint32_t bytes, uint32_t count), uint32_t dst, uint32_t bytes, uint32_t count) {
    return from2B760(dst, bytes, count);
}
