// 1.13c D2Game+0x69940 -- CheckItemClassEA. EAX = unit, ECX = item class. Written from the 1.13c disassembly.
// Class 0xEA with a non-null EAX asks 0xA556(unit) and then 0xA56E(that), and tests only AL of the answer:
// zero refuses (returns 0); everything else returns 1.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t f69940(HookEnv& e, uint32_t eax, uint32_t ecx)
{
    if (ecx == 0xEA && eax != 0) {
        const uint32_t v = e.h(0xA556, 0, 0, 0, 0, 0, eax);
        if ((e.h(0xA56E, 0, 0, 0, 0, 0, v) & 0xFF) == 0) return 0;
    }
    return 1;
}
}
