// 1.13c D2Client+0xAC0B0: packet handler, ECX = packet. Copies the packet's first seven bytes (dword, word, byte) into a stack record and hands
// its address to 0x45A50 in EDX (no stack words). EAX is that callee's result.
#include "d2c_seam.h"
extern "C" uint32_t d2c_ac0b0(const D2cSeam* s, const uint8_t* pkt) {
    uint8_t rec[8] = {};   // byte 7 is stack residue in retail; the capture masks it
    std::memcpy(rec, pkt, 7);
    return (uint32_t)(*s)(0x45a50, 0, 0, (uint64_t)(uintptr_t)rec);
}
