// 0x62640 (EAX = quest record, ECX = unit, stack = game, ret 4): #10003 sets states 0xD and 1 of quest 6 on the record, then raises the unit's level counter
// on its client data: client = unit type 0 ? dword at #10920(unit)+0x9C : 0. The client word at +0xA holds a 5-bit value in bits 8-12 and a flag in bit 5: step =
// 4 + (bit 5 of the low byte), so step * difficulty + 1 is stored there (keeping bits 0-7 and 13-15) when it is not below the current value.
// The function has no result of its own; EAX at exit is the step when the store is skipped, else the 32-bit value whose low word was stored.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x62640
uint32_t d2game_62640_q32(const Call& X, uint32_t rec, uint32_t unit, uint32_t game) {
    qSet(X, rec, 6, 0xd);
    qSet(X, rec, 6, 1);
    uint32_t cl = 0;
    if (unit && rd32(unit, 0) == 0) cl = rd32(X(kO10920, 0, 0, 0, 0, 0, unit), 0x9c);
    uint32_t diff = rd8(game, 0x6d);
    uint32_t w = rd16(cl, 0xa);
    uint32_t step = ((w & 0x20) | 0x80) >> 5;
    if ((int32_t)(step * diff + 1) < (int32_t)((w >> 8) & 0x1f)) return step;
    const uint32_t v = ((step * diff + 1) << 8) | (w & 0xe0ff);
    wr16(cl, 0xa, (uint16_t)v);
    return v;
}
