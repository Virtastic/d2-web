// 1.13c D2Client+0xAE1E0: server packet relay with two 16-bit fields. ECX = packet; the tail callee 0x4A350 gets EAX = the packet
// pointer's high half over the word at +1 (a word move into AX) and ECX = the word at +3; its result is returned.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_ae1e0(uint32_t pk, const Env& e) {
    return e.H(0x4a350, 0, 0, 0, 0, 0, (pk & 0xffff0000u) | W16(pk + 1), W16(pk + 3));
}
