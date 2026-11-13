// 1.13c D2Client+0xAFA70: server packet {?, byte, id dword, ?, length byte at +6, data from +7}: 0x4B8A0 gets EAX = the id's high
// 24 bits over the byte at +1 (a byte move into AL), stack id, data pointer, length; its result is returned.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_afa70(uint32_t pk, const Env& e) {
    const uint32_t id = e.R(pk + 2);
    return e.H(0x4b8a0, id, pk + 7, B(pk + 6), 0, 0, (id & ~0xffu) | B(pk + 1));
}
