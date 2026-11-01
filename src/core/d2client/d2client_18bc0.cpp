// 1.13c D2Client+0x18BC0: EDI = unit (type 0/1 with a +0x64 block); value = 0xD for a player, else max(ord(unit), monster row +0x11E) ; + s0, floored at 0, handed to the 0xA8B60 setter (ret 4)
#include "cl17.h"
extern "C" uint32_t d2c_18bc0(uint32_t unit, uint32_t s0, const cl::Env& e) {
    if (!unit) return 0;
    uint32_t t = e.R(unit);
    if (t > 1) return t;
    if (!e.R(unit + 0x64)) return t;
    uint32_t v;
    if (t == 0) v = 0xd;
    else {
        uint32_t row = e.H(0x1260, 0, 0, 0, 0, 0, e.R(unit + 4));
        if (!row) return 0;
        v = e.H(0xcdae, unit) & 0xff;
        uint32_t c = e.R(row + 0x11e) & 0xff;
        if ((int32_t)v < (int32_t)c) v = c;
    }
    v += s0;
    if ((int32_t)v <= 0) v = 0;
    return e.H(0xa8b60, 0, 0, 0, 0, 0, v);
}
