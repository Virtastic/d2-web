// 1.13c D2Common+0x315E0 -- UNITS_TestCollisionByCoordinates.
// Retail (decompile 0x315E0 + disassembly): a null unit or a null room (2FE10) returns 0. The unit's sub-struct
// (unit+0x2C) supplies ux and uy: types 2 and 4..5 read dwords at +0xC/+0x10 with no null check; the other types
// read u16 at +2/+6, zero when the sub-struct is null. size = 30F80(unit). The result is 30730 with EAX = x,
// ECX = ux, EDX = 2 and stack uy, size, y, room, flags. Retail pushes flags as a full dword, so the callee gets it
// unmodified (30730 reads the low word itself).
#include "d2common_315e0.h"

extern "C" uint32_t D2Common_315E0_CollisionInjected(const D2Collision315E0Callees* c, uint32_t unit,
                                                    uint32_t x, uint32_t y, uint32_t flags)
{
    if (!unit) {
        return 0;
    }

    const uint32_t room = c->getRoom(c->ctx, unit);
    if (!room) {
        return 0;
    }

    const uint8_t* u = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(unit));
    const uint32_t type = *reinterpret_cast<const uint32_t*>(u + 0x00);
    const uint32_t sub = *reinterpret_cast<const uint32_t*>(u + 0x2C);

    uint32_t ux;
    uint32_t uy;
    if (type == 2 || (type > 3 && type < 6)) {
        const uint8_t* s = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(sub));
        ux = *reinterpret_cast<const uint32_t*>(s + 0x0C);
        uy = *reinterpret_cast<const uint32_t*>(s + 0x10);
    } else if (!sub) {
        ux = 0;
        uy = 0;
    } else {
        const uint8_t* s = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(sub));
        ux = *reinterpret_cast<const uint16_t*>(s + 0x02);
        uy = *reinterpret_cast<const uint16_t*>(s + 0x06);
    }

    const uint32_t size = c->getSize(c->ctx, unit);
    return c->test(c->ctx, x, ux, uy, size, y, room, flags);
}
