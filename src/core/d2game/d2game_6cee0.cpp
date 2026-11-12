// 1.13c D2Game+0x6CEE0: the well's operate function (ECX = operate record: +0 game, +4 object, +8 player; returns 0). The vendored
// OBJECTS_OperateFunction22_Well (d2moo ObjMode.cpp) carries the same rule; this is its seam form. With charges left (object data
// +0x14 -> byte +4) and 0x6A560 (EAX = objects.txt row #B156(object), ESI = player; game) healing someone: charges - 1 is compared
// with the byte 2 * row byte +0x180 and, when not above it and divisible by that byte halved, the mode becomes
// #A4A2(object, 2 - charges / row +0x180); the charge byte is stored and 0x8EA80 (ECX = game, EDX = object; 2, game frame +0xA8 +
// row +0x178 + 1, 0, 0, 0) sets the next think event.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_6cee0(uint32_t op, const D2Seam* e)
{
    const uint32_t obj = d2rd32(op + 4);
    const uint32_t txt = e->call(0xB156, 0, 0, 0, 0, 0, obj);
    uint8_t n = d2rd8(d2rd32(obj + 0x14) + 4);
    if (!n) return 0;
    const uint32_t game = d2rd32(op);
    if (!e->call(0x6A560, txt, 0, 0, 0, d2rd32(op + 8), game)) return 0;
    --n;
    const uint8_t twice = (uint8_t)(d2rd8(txt + 0x180) << 1);
    if (n <= twice && (int32_t)n % (int32_t)(twice >> 1) == 0)
        e->call(0xA4A2, 0, 0, 0, 0, 0, d2rd32(op + 4), (uint32_t)(2 - (int32_t)n / (int32_t)d2rd32(txt + 0x180)));
    d2wr8(d2rd32(d2rd32(op + 4) + 0x14) + 4, n);
    e->call(0x8EA80, 0, game, d2rd32(op + 4), 0, 0, 2, d2rd32(game + 0xA8) + d2rd32(txt + 0x178) + 1, 0, 0, 0);
    return 0;
}
