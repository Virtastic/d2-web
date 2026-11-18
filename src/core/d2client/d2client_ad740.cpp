// 1.13c D2Client+0xAD740: server packet relay. ECX = packet {?, byte, dword, word}, EDX = the caller's EDX; the callee 0x23F20 takes ECX = the dword at +2,
// EDX = the caller's EDX with its low byte replaced by the packet byte at +1, and the zero-extended word at +6 as its one stack word; its return stays in EAX.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_ad740(uint32_t pk, uint32_t edx, const Env& e) {
    return e.H(0x23f20, W16(pk + 6), 0, 0, 0, 0, 0, e.R(pk + 2), (edx & ~0xffu) | B(pk + 1));
}
