// 1.13c D2Game+0xC3480 -- player enters a level flagged for quest 0xC: sets the five player flags and clears the rest
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fc3480(NE& e, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t buf = e.qbuf(game, unit);
    uint32_t r = e.fl(buf, 0xc, 0);
    if (r == 1) return r;
    r = e.fl(buf, 0xc, 1);
    if (r == 1) return r;
    r = e.imp(0xA478, unit);
    if (!r) return r;
    r = e.imp(0xA556, r);
    if (!r) return r;
    r = e.imp(0xA56E, r);
    if ((r & 0xff) != 1) return r;
    e.setf(buf, 0xc, 0xd); e.setf(buf, 0xc, 1); e.setf(buf, 0xc, 0);
    e.imp(0xA574, buf, 0xc);
    e.setf(buf, 0xc, 8);
    return e.setf(buf, 0xc, 7);
}
}
