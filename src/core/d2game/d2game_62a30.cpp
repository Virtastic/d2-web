// 0x62A30 (ECX = unit; plain ret): send the 6-byte packet 5D 04 01 00 00 00 to the unit's client: D2Common #10920 gives the player record when the
// unit's type dword is 0, the client is its dword +0x9C (else 0); 0x8A3E0 (EAX = client, stack packet, 6; ret 8).
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x62A30
void d2game_62A30_q32(const Call& X, uint32_t unit) {
    uint8_t pkt[8] = { 0x5d, 0x04, 0x01, 0, 0, 0, 0, 0 };
    uint32_t client = 0;
    if (unit && rd32(unit, 0) == 0) client = rd32(X(kO10920, 0, 0, 0, 0, 0, unit), 0x9c);
    X(0x8A3E0, client, 0, 0, 0, 0, (uint32_t)(uintptr_t)pkt, 6);
}
