// 1.13c D2Game+0x422B0 -- ends a unit mode: the pending step, the mode gate, then the mode change and its follow-ups
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f422b0(NE& e, uint32_t unit, uint32_t m, uint32_t s0)
{
    e.h(0x37110, 0, 0, 0, e.rd32(unit + 0x80), 0, 1, m);
    const uint32_t t = e.rd32(unit), md = e.rd32(unit + 0x10);
    bool gate = true;
    if (!(e.rd8(unit + 0xc6) & 1)) {
        if (t == 0) gate = md == 0 || md == 0x11;
        else if (t == 1) gate = md == 0 || md == 0xc;
    }
    uint32_t eax;
    if (gate) { eax = e.imp(0xA7E4, unit, m); if (eax) return eax; }
    e.imp(0xA3CA, unit, m, 0);
    e.imp(0xA59E, unit, 0xB2000000u | 0xfc428, 0x1a9);
    eax = e.imp(0xAEB6, unit);
    if (!e.rd32(unit)) eax = e.h(0x96FA0, 0, 0, 0, 0, 0, e.rd32(unit + 0x80), unit);
    return eax;
}
}
