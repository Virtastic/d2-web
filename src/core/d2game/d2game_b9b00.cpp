// 1.13c D2Game+0xB9B00 (retail asm: ECX = game, EDX = unit, one ignored stack word, ret 4, returns 0): when quest flag 0x1A/0xD of the
// unit's difficulty slot is set, sends packet 5D 17 02 00 00 00 through 0x8A3E0 (EAX = the player record's +0x9C, or 0; stack: buffer, 6),
// then rebinds the unit (+0x6E = 0x4B, +0x70 = itself, #10158 thunk 0xA4DE, flags +0xC4 |= 0x400). A null unit reaches the fatal
// path (#10265, #10024, _exit(-1)), which a capture cannot return from: only non-null units are recorded.
#include "d2game_kmem.h"

namespace d2g_b9b00 {
struct Env {
    D2gMem m;
    uint32_t (*ord10920)(uint32_t unit);                              // 0xA4C0
    uint32_t (*ord10174)(uint32_t qh, uint32_t a, uint32_t b);        // 0xA568
    void     (*send8A3E0)(uint32_t player, const uint8_t* pkt, uint32_t len);   // 0x8A3E0, EAX = player; the 6 bytes at pkt
    void     (*ord10158)(uint32_t unit);                              // 0xA4DE
    void     (*fatal)(uint32_t unit);                                 // unit == 0 after the packet: #10265, #10024, _exit(-1)
};
inline uint32_t run(const Env& e, uint32_t game, uint32_t unit) {
    const D2gMem& m = e.m;
    uint32_t qd = m.r32(e.ord10920(unit) + 0x10 + m.r8(game + 0x6D) * 4);
    if (e.ord10174(qd, 0x1A, 0xD) == 0) return 0;
    uint32_t pl = 0;
    if (unit && m.r32(unit) == 0) pl = m.r32(e.ord10920(unit) + 0x9C);
    const uint8_t pkt[6] = { 0x5D, 0x17, 0x02, 0x00, 0x00, 0x00 };   // a stack local in retail (the call logs its address as a stack pointer)
    e.send8A3E0(pl, pkt, 6);
    if (!unit) { e.fatal(unit); return 0; }
    m.w16(unit + 0x6E, 0x4B);
    m.w32(unit + 0x70, unit);
    e.ord10158(unit);
    m.w32(unit + 0xC4, m.r32(unit + 0xC4) | 0x400);
    return 0;
}
}
