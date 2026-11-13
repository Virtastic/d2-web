// 1.13c D2Game+0x57830 -- per-player completion of the quest 0x26 on entering act 5
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f57830(NE& e, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t buf = e.qbuf(game, unit);
    uint32_t eax;
    if ((eax = e.fl(buf, 0x26, 0)) == 1) return eax;
    if ((eax = e.fl(buf, 0x26, 0xe)) == 1) return eax;
    if ((eax = e.fl(buf, 0x26, 1)) == 1) return eax;
    if ((eax = e.fl(buf, 0x26, 0xd)) == 1) return eax;
    if (!(eax = e.imp(0xA478, unit))) return eax;
    if (!(eax = e.imp(0xA556, eax))) return eax;
    eax = e.imp(0xA56E, eax);
    if ((eax & 0xff) != 4) return eax;
    eax = e.fl(buf, 0x25, 0);
    if (!eax) { eax = e.fl(buf, 0x25, 1); if (!eax) return eax; }
    if ((eax = e.fl(buf, 0x26, 0))) return eax;
    if ((eax = e.fl(buf, 0x26, 1))) return eax;
    e.setf(buf, 0x26, 1);
    return e.setf(buf, 0x26, 0xd);
}
}
