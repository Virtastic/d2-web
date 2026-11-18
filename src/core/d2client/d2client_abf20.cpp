// 1.13c D2Client+0xABF20 -- packet handler (ECX = packet): a zero byte at +1 hands (the word at +4, the bytes from +6)
// to the two-word routine and returns its answer; otherwise that byte is left in AL above the caller's EAX.
#include "cl_tbl.h"
extern "C" uint32_t d2c_abf20(uint32_t eax, uint32_t pkt, const cl::Env& e) {
    const uint32_t b = e.R8(pkt + 1);
    if (b) return (eax & ~0xffu) | b;
    return e.H(0xc514, clt::P16(e, pkt + 4), pkt + 6);
}
