// 1.13c D2Client+0xAC780: packet handler, ECX = packet (unit id +2, flag byte +6, word +7, dword +9). Finds the type-0 unit in the table at D2Client+0x10A608
// (128 chains by id & 0x7F: id at +0xC, next at +0xE4); none ends it with EAX 0. A unit whose type dword is nonzero is retail's fatal assert (0xBF92(0x5A),
// 0xBF80, 0x3664). Else 0xC50E (flag set) or 0xC52C (clear), both stdcall 3 with (unit, word, dword); EAX is its result.
#include "d2c_seam.h"
extern "C" uint32_t d2c_ac780(const D2cSeam* s, const uint32_t* tbl, const uint8_t* pkt) {
    uint32_t id, d9; uint16_t w7; std::memcpy(&id, pkt + 2, 4); std::memcpy(&w7, pkt + 7, 2); std::memcpy(&d9, pkt + 9, 4);
    uint32_t a = tbl[id & 0x7f];
    while (a) {
        const uint8_t* u = (const uint8_t*)(uintptr_t)a; uint32_t uid; std::memcpy(&uid, u + 0xc, 4);
        if (uid == id) break;
        std::memcpy(&a, u + 0xe4, 4);
    }
    if (!a) return 0;
    uint32_t type; std::memcpy(&type, (const void*)(uintptr_t)a, 4);
    if (type) {   // never returns in retail
        const uint32_t rc = (uint32_t)(*s)(0xbf92, 0, 0, 0, 0, 0, 0x5a);
        (*s)(0xbf80, 0, 0, 0, 0, 0, (uint32_t)s->mod(0xd359f), rc);
        (*s)(0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
        return 0;
    }
    return (uint32_t)(*s)(pkt[6] ? 0xc50e : 0xc52c, 0, 0, 0, 0, 0, a, w7, d9);
}
