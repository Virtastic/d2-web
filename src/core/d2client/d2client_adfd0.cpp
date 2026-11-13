// 1.13c D2Client+0xADFD0: server packet relay. ECX = packet {?, byte, dword}; the callee 0x49370 takes ECX = the dword at +2 and
// EBX = the caller's EBX with its low byte replaced by the packet byte at +1 (a byte move into BL); its result is returned as is.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_adfd0(uint32_t pk, uint32_t ebx, const Env& e) {
    return e.H(0x49370, 0, 0, 0, 0, 0, 0, e.R(pk + 2), 0, (ebx & ~0xffu) | B(pk + 1));
}
