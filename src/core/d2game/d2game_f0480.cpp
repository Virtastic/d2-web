// D2Game 0xF0480: first contact between two players p1 and p2 (the pair-relation initialiser behind the 0x2DB48 callback table).
// Convention (disassembly of the 1.13c bytes): ECX = game, EDX = p1, stack p2 (ret 4). Relation node = {other id +0, flags +4, state +0xC, next +0x10}, list
// head at the player data's +0x38 (player data = D2Common 0xA4C0(unit), stdcall).
// In order: 0xF02F0(EAX = p2, ECX = p1; game), 0xF02F0(EAX = p1, ECX = p2; game), 0x8C570(EAX = p1, ESI = p2); p1's node for p2's id loses its state (+0xC = 0) when
// p1 != p2; v = 1 when p1, p1+0x80 and type 0 all hold and 0x9BEB0(EAX = p1, EDI = [p1+0x80]) has a low word other than 0xFFFF (else v = 0, no call);
// 0xF0220(ESI = p1, EDI = p2; v); 0xF0130(EAX = p1, EDI = p2); 0xF0130(EAX = p2, EDI = p1); then each node of p1's list goes to p2's client
// ((0xA4C0(p2) + 0x9C) when p2 is type 0, else 0) as an 11-byte 0x8C packet {0x8C, p1 id (-1 when null), node id, node flags word} through 0x8A3E0(EAX = client; &packet, 11).
#include "d2game_seam4e.h"

// 1.13c D2Game+0xF0480
extern "C" void d2game_f0480(uint32_t game, uint32_t p1, uint32_t p2, const D2Seam* sm)
{
    const D2Seam& s = *sm;
    const uint32_t id1 = p1 ? d2rd32(p1 + 0xC) : 0xFFFFFFFFu;
    s.call(0xF02F0, p2, p1, 0, 0, 0, game);
    s.call(0xF02F0, p1, p2, 0, 0, 0, game);
    s.call(0x8C570, p1, 0, 0, 0, p2);
    const uint32_t pd = s.call(0xA4C0, 0, 0, 0, 0, 0, p1);
    const uint32_t id2 = p2 ? d2rd32(p2 + 0xC) : 0xFFFFFFFFu;
    if (p1 != p2) {
        uint32_t n = d2rd32(pd + 0x38);
        while (n && d2rd32(n) != id2) n = d2rd32(n + 0x10);
        if (n) d2wr32(n + 0xC, 0);
    }
    uint32_t v = 0;
    if (p1 && d2rd32(p1 + 0x80) && d2rd32(p1) == 0) {
        const uint32_t r = s.call(0x9BEB0, p1);
        s.edi(0x9BEB0, d2rd32(p1 + 0x80));
        v = (r & 0xFFFF) != 0xFFFF;
    }
    s.call(0xF0220, 0, 0, 0, 0, p1, v);
    s.edi(0xF0220, p2);
    s.call(0xF0130, p1);
    s.edi(0xF0130, p2);
    s.call(0xF0130, p2);
    s.edi(0xF0130, p1);
    const uint32_t pd1 = s.call(0xA4C0, 0, 0, 0, 0, 0, p1);
    for (uint32_t n = d2rd32(pd1 + 0x38); n; n = d2rd32(n + 0x10)) {
        uint8_t pkt[16] = {};
        pkt[0] = 0x8C;
        d2wr32((uint32_t)(uintptr_t)pkt + 1, id1);
        d2wr32((uint32_t)(uintptr_t)pkt + 5, d2rd32(n));
        d2wr16((uint32_t)(uintptr_t)pkt + 9, d2rd16(n + 4));
        uint32_t client = 0;
        if (p2 && d2rd32(p2) == 0) client = d2rd32(s.call(0xA4C0, 0, 0, 0, 0, 0, p2) + 0x9C);
        s.call(0x8A3E0, client, 0, 0, 0, 0, (uint32_t)(uintptr_t)pkt, 11);
    }
}
