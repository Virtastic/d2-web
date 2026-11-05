// ECX = unit. Sends the 6-byte message 5D 21 20 00 00 00 to the unit's client: the client is the player record's +0x9C word for a
// player unit (type 0), none (0) for any other unit or a null one.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x59180
void d2game_59180_q32(const Call& X, uint32_t unit) {
    uint32_t client = 0;
    if (unit && rd32(unit, 0) == 0) client = rd32(X(kO10920, 0, 0, 0, 0, 0, unit), 0x9c);
    uint8_t msg[6] = { 0x5d, 0x21, 0x20, 0, 0, 0 };
    X(0x8A3E0, client, 0, 0, 0, 0, (uint32_t)(uintptr_t)msg, 6);
}
