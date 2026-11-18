// 1.13c D2Client+0xAC970 purgeCorpseObjects, written from the disassembly: EBX = room. With a room and its unit list (+0x60):
// x / y are the signed shorts behind the two pointer chains at [[room+0xA8]+8] and [[room+0xA8]+0xC] (0 where a link is null).
// Walks the list (10460 first, 10464 next, read before the node is handled; 11147 the node's record): a record of type 4 for which
// 11160 answers nonzero (and writes a word into the local passed) is removed (11127 room, record) and re-placed (10164 room, record,
// that word). Then 10546(room, x, -1) and 10978(room, y, -1).
#include "cl_genv.h"
using namespace cl;
extern "C" void d2c13_ac970(uint32_t room, const Env& e) {
    if (!room) return;
    const uint32_t list = e.R(room + 0x60);
    if (!list) return;
    const uint32_t ps = e.R(room + 0xa8);
    const uint32_t px = e.R(ps + 8), py = e.R(ps + 0xc);
    const uint32_t x = px ? (uint32_t)(int32_t)(int16_t)W16(e.R(px)) : 0;
    const uint32_t y = py ? (uint32_t)(int32_t)(int16_t)W16(e.R(py)) : 0;
    uint32_t n = e.H(0xc4f0, list);
    uint32_t l0 = 0;
    while (n) {
        const uint32_t nx = e.H(0xc568, n);
        const uint32_t rec = e.H(0xc526, n);
        l0 = 0;
        if (rec && e.R(rec) == 4 && e.H(0xc51a, rec, Stk(&l0))) {
            e.H(0xc544, room, rec);
            e.H(0xc4fc, room, rec, l0);
        }
        n = nx;
    }
    e.H(0xc50e, room, x, 0xffffffffu);
    e.H(0xc52c, room, y, 0xffffffffu);
}
