// 1.13c D2Client+0xAC880: packet handler, ECX = packet (unit id +2, record count byte +1, records of word + byte from +6). Same unit table walk as 0xAC780
// (fatal 0x5A on a nonzero type); then per record 0xC550 (stdcall 6) with (unit, word, byte, 0, "..\Source\D2Client\GAME\SCmd.cpp", 0xAB7).
// EAX is the last call's result, or the unit id when nothing was called.
#include "d2c_seam.h"
extern "C" uint32_t d2c_ac880(const D2cSeam* s, const uint32_t* tbl, const uint8_t* pkt) {
    uint32_t id; std::memcpy(&id, pkt + 2, 4);
    uint32_t a = tbl[id & 0x7f];
    while (a) {
        const uint8_t* u = (const uint8_t*)(uintptr_t)a; uint32_t uid; std::memcpy(&uid, u + 0xc, 4);
        if (uid == id) break;
        std::memcpy(&a, u + 0xe4, 4);
    }
    if (!a) return id;
    uint32_t type; std::memcpy(&type, (const void*)(uintptr_t)a, 4);
    if (type) {   // never returns in retail
        const uint32_t rc = (uint32_t)(*s)(0xbf92, 0, 0, 0, 0, 0, 0x5a);
        (*s)(0xbf80, 0, 0, 0, 0, 0, (uint32_t)s->mod(0xd359f), rc);
        (*s)(0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
        return 0;
    }
    uint32_t r = id;
    for (uint32_t i = 0; i < pkt[1]; i++) {
        uint16_t w; std::memcpy(&w, pkt + 6 + 3 * i, 2);
        r = (uint32_t)(*s)(0xc550, 0, 0, 0, 0, 0, a, w, pkt[8 + 3 * i], 0, s->mod(0xd4034), 0xab7);
    }
    return r;
}
