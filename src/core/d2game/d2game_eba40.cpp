// D2Game attach-player-to-client over retail-layout (32-bit) memory. Written from the 1.13c disassembly.
// Convention: EAX = client, ECX = player unit, [esp+4] = game (ret 4). Client +0x16C unit type, +0x170 unit id, +0x174 unit;
// the player data (+0x9C client) gets the client and its +0x50..+0x5C words cleared. The name copy callee takes (unit, client+0xD);
// the client-list callee takes ESI = game+0xD0. The null-client, null-unit and non-player asserts (error report plus exit in retail) return here.
#include "d2game_seam4e.h"

// 1.13c D2Game+0xEBA40
extern "C" void d2game_eba40(uint32_t client, uint32_t unit, uint32_t game, const D2Seam* e)
{
    if (!client) return;
    if (!unit) { d2wr32(client + 0x174, 0); return; }
    d2wr32(client + 0x16C, d2rd32(unit));
    d2wr32(client + 0x170, d2rd32(unit + 0xC));
    d2wr32(client + 0x174, unit);
    if (d2rd32(unit)) return;
    d2wr32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C, client);
    e->call(0xA40C, 0, 0, 0, 0, 0, unit, client + 0xD);
    e->call(0xE0110, 0, 0, 0, 0, game + 0xD0);
    e->call(0xA4A2, 0, 0, 0, 0, 0, unit, 1);
    const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
    d2wr32(pd + 0x5C, 0); d2wr32(pd + 0x50, 0); d2wr32(pd + 0x58, 0); d2wr32(pd + 0x54, 0);
}
