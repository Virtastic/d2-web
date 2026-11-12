// 1.13c D2Game+0x15570 -- ECX = unit, EDX = out, one ignored stack argument. A null unit, or one for which the D2Common import 0xA688 answers non-zero, stores 1 through out. EAX is the import's answer, or untouched for a null unit.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f15570(GE& e, uint32_t eax, uint32_t unit, uint32_t out, uint32_t)
{
    if (unit) { eax = e.h(0xA688, 0, 0, 0, 0, 0, unit); if (!eax) return 0; }
    e.wr32(out, 1);
    return eax;
}
}
