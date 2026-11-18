// 1.13c D2Client+0xAD0B0: packet handler, ECX = packet (type byte +1, unit id +2, byte +6). Unit through 0xA4E20 (as 0xADAA0); none, the local player
// (local = D2Client+0x11BBFC), or bit 9 of unit+0xC4 ends it (EAX = the unit). Unit type 1: 0xC40C(unit, 6, byte << 8, 0) (stdcall 4). Else 0x9D630 (EDX = id)
// gives a record or 0; a record gets +0x14 = (byte * 100) >> 7 (arithmetic, low byte).
#include "d2c_seam.h"
extern "C" uint32_t d2c_ad0b0(const D2cSeam* s, uint32_t local, const uint8_t* pkt) {
    const uint32_t type = pkt[1]; uint32_t id; std::memcpy(&id, pkt + 2, 4);
    const uint64_t u = (*s)(0xa4e20, id & 0x7f, id, s->mod(0x10a608) + (type << 9), 0, 0, type);
    if (!u || (uint32_t)u == local) return (uint32_t)u;
    uint32_t flags, utype; std::memcpy(&flags, (const void*)(uintptr_t)(u + 0xc4), 4); std::memcpy(&utype, (const void*)(uintptr_t)u, 4);
    if (flags >> 9 & 1) return (uint32_t)u;
    if (utype == 1) return (uint32_t)(*s)(0xc40c, 0, 0, 0, 0, 0, u, 6, (uint32_t)pkt[6] << 8, 0);
    const uint64_t rec = (*s)(0x9d630, 0, 0, id);
    if (!rec) return 0;
    int32_t v = (int32_t)pkt[6] * 100; v = (v + ((v >> 31) & 0x7f)) >> 7;
    const uint32_t out = (uint8_t)v;
    std::memcpy((void*)(uintptr_t)(rec + 0x14), &out, 4);
    return out;
}
