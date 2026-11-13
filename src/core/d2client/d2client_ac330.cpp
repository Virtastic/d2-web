// 1.13c D2Client+0xAC330: packet handler, ECX = packet. Hands 0xCE0E (stdcall 4): the module dword at D2Client+0x11973F, the packet from byte 1,
// 0x60 and 0. EAX is that callee's result.
#include "d2c_seam.h"
extern "C" uint32_t d2c_ac330(const D2cSeam* s, uint32_t g, const uint8_t* pkt) {
    return (uint32_t)(*s)(0xce0e, 0, 0, 0, 0, 0, g, (uint64_t)(uintptr_t)(pkt + 1), 0x60, 0);
}
