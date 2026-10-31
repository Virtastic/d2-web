// 1.13c D2CMP+0x11B0: EDI = owner. While the owner's first entry (+8) is a live pointer (> 0),
// unlink it (0x1020) and free it to Storm under "__AUTCELENTRY__".
#include "d2cmp_guest.h"
using namespace d2cmpg;
extern "C" void d2cmp_1020(uint8_t* m, uint32_t node);
// 1.13c D2CMP+0x11B0
extern "C" void d2cmp_11b0(uint8_t* m, uint32_t obj, const D2CmpGuestEnv* e) {
    uint32_t p;
    while ((int32_t)(p = rd(m, obj + 8)) > 0) {
        d2cmp_1020(m, p);
        e->free(m, p, kTagCelEntry, -2, 0);
    }
}
