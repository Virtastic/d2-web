// 1.13c D2Game+0xDA480 -- item mode notify (table slot 0xFB11C). ECX = game, EDX = unit.
// A8A4 (D2Common 10202; unit, 0x100, 0x15A7, ItemMode.cpp) nonzero skips the first arm. First arm: A970 (10865) nonzero ->
// DA350(eax 0xFC, ecx 0x48; game, unit, AA00 (11116) of unit); a nonzero answer ends. Second arm: A940 (10048) nonzero ->
// DA350(eax 0xFD, ecx 0x46; game, unit, AA06 (10463) of unit). No 1.10f counterpart.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
void fda480(GE& e, uint32_t game, uint32_t unit)
{
    const uint32_t file = 0xB2000000u | 0xFB14C;
    bool second = true;
    if (e.h(0xA8A4, 0, 0, 0, 0, 0, unit, 0x100, 0x15A7, file) == 0 && e.h(0xA970, 0, 0, 0, 0, 0, unit) != 0) {
        const uint32_t a = e.h(0xAA00, 0, 0, 0, 0, 0, unit);
        if (e.h(0xDA350, 0xFC, 0x48, 0, 0, 0, game, unit, a) != 0) second = false;
    }
    if (second && e.h(0xA940, 0, 0, 0, 0, 0, unit) != 0) {
        const uint32_t a = e.h(0xAA06, 0, 0, 0, 0, 0, unit);
        e.h(0xDA350, 0xFD, 0x46, 0, 0, 0, game, unit, a);
    }
}
}
