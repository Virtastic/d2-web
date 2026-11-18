// 1.13c D2Client+0xADAA0: packet handler, ECX = packet (type byte +1, unit id +2, word +6). Finds the unit through 0xA4E20
// (EAX = id & 0x7F, ECX = id, EDX = the type's bucket row, stack type); none, or a word not below the table count, ends it. Else
// 0x6B300(unit, 2, 0, 0, 0, 0, 0) with EAX = the word, and for words 0x97 / 0x98 the sound 0x88A70(unit, 0, 0, 0) with EBX = 0x18C / 0x18D.
// EAX is what the last step leaves: the lookup's 0, the table count, or the last callee's result.
#include "d2c_seam.h"
extern "C" uint32_t d2c_adaa0(int32_t count, const D2cSeam* s, const uint8_t* pkt) {
    const uint32_t type = pkt[1]; uint32_t id; std::memcpy(&id, pkt + 2, 4); uint16_t w; std::memcpy(&w, pkt + 6, 2);
    const uint64_t u = (*s)(0xa4e20, id & 0x7f, id, s->mod(0x10a608) + (type << 9), 0, 0, type);
    if (!u) return 0;
    if ((int32_t)w >= count) return (uint32_t)count;
    uint32_t r = (uint32_t)(*s)(0x6b300, w, 0, 0, 0, 0, u, 2, 0, 0, 0, 0, 0);
    if (w == 0x97) r = (uint32_t)(*s)(0x88a70, 0, 0, 0, 0x18c, 0, u, 0, 0, 0);
    else if (w == 0x98) r = (uint32_t)(*s)(0x88a70, 0, 0, 0, 0x18d, 0, u, 0, 0, 0);
    return r;
}
