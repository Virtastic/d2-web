// 1.13c D2Client+0x26820: belt slot highlight (ESI = colour index, stack = slot).
#include "d2client_callee.h"
// 1.13c D2Client+0x26820
extern "C" void d2c_26820(const D2cSvc& v, uint32_t sel, uint32_t slot, uint32_t belt /*0xF5384*/, uint32_t inv /*0x11B99C*/, const uint8_t* colors /*0x11A7D0*/) {
    d2c_call(v, 0x265e0, 0, 0, 0, 0, sel);
    if (sel == 2) return;
    uint32_t rc[4];   // local rect: {left, ?, top, ?}
    d2c_call(v, 0xcf46, 0, 0, 0, 0, sel, belt, inv, (uint32_t)(uintptr_t)rc, slot);   // D2Common #10333 (belt, inv, &rect, slot)
    d2c_call(v, 0xd0ba, 0, 0, 0, 0, sel, rc[0], rc[2], rc[0] + 0x1d, rc[2] + 0x1d, colors[sel], 0);   // D2Gfx #10014 fill rect
}
