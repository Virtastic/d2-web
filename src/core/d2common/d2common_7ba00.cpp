// stand-alone 32-bit layout twin of D2Common 0x7BA00 (the proven row runs DRLGACTIVATE_TestRoomCanUnTile): room-node gate, node in EDX. A node flagged 0x400000 (+0x28) fails; its
// owner (+0x58) and the owner's chain (+0x1B4) must be set and the chain's bit 0 (+0x8C) clear (retail: Fog fatal + _exit(-1),
// here 0). A node whose byte +0x44 is at most 1 fails; when the owner id (+0x1D0) is one of 1/0x28/0x4B/0x67/0x6D (0x2D320) or
// 0x78, every node on the owner's list (+0x10, next +0x24) must have byte +0x44 above 1. Memory reads go through rd so the live
// probe can answer an uncarried address as 0.
#include <cstdint>

extern "C" uint32_t D2Common_7BA00_NodeGate(uint32_t node, uint32_t (*rd)(uint32_t))
{
    if (rd(node + 0x28) & 0x400000u) return 0;
    const uint32_t owner = rd(node + 0x58);
    if (!owner) return 0;                                  // fatal 0x42C in retail
    const uint32_t chain = rd(owner + 0x1B4);
    if (!chain) return 0;                                  // fatal 0x42D in retail
    if (rd(chain + 0x8C) & 1u) return 0;                   // fatal 0x2E7 in retail
    if ((rd(node + 0x44) & 0xFFu) <= 1) return 0;
    const uint32_t id = rd(owner + 0x1D0);
    const bool walk = id == 1 || id == 0x28 || id == 0x4B || id == 0x67 || id == 0x6D || id == 0x78;
    if (walk)
        for (uint32_t n = rd(owner + 0x10); n; n = rd(n + 0x24))
            if ((rd(n + 0x44) & 0xFFu) <= 1) return 0;
    return 1;
}
