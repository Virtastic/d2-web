// 1.13c D2Client+0xAC720: packet handler, ECX = packet. Byte +1 set: 0x218C0(dword +5, word +3) (ret 8) with EBX = dword +9 and ESI = byte +2;
// clear: tail-jumps 0x21840 with EDX = dword +9. EAX is the callee's result either way.
#include "d2c_seam.h"
extern "C" uint32_t d2c_ac720(const D2cSeam* s, const uint8_t* pkt) {
    uint32_t d9, d5; uint16_t w3; std::memcpy(&d9, pkt + 9, 4); std::memcpy(&d5, pkt + 5, 4); std::memcpy(&w3, pkt + 3, 2);
    if (pkt[1]) return (uint32_t)(*s)(0x218c0, 0, 0, 0, d9, pkt[2], d5, w3);
    return (uint32_t)(*s)(0x21840, 0, 0, d9);
}
