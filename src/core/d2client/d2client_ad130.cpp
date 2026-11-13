// 1.13c D2Client+0xAD130: packet handler, ECX = packet (flag byte +1, id +2, dword +6). Flag set: 0x9D760(dword) (ret 4) with EBX = EDX = id;
// clear: 0x9D700 with EDX = id. EAX is the callee's result.
#include "d2c_seam.h"
extern "C" uint32_t d2c_ad130(const D2cSeam* s, const uint8_t* pkt) {
    uint32_t id, v; std::memcpy(&id, pkt + 2, 4); std::memcpy(&v, pkt + 6, 4);
    if (pkt[1]) return (uint32_t)(*s)(0x9d760, 0, 0, id, id, 0, v);
    return (uint32_t)(*s)(0x9d700, 0, 0, id);
}
