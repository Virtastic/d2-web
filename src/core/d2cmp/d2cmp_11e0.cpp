// 1.13c D2CMP+0x11E0 (stdcall list, extra, flags; EDI = mode): allocate an AUTCELENTRY node of
// extra+0x24 bytes (Storm flags | 8), clear its link, and if mode != 0 link it into `list` (0x1740).
#include "d2cmp_guest.h"
using namespace d2cmpg;
extern "C" void d2cmp_1740(uint8_t* m, uint32_t target, uint32_t list, uint32_t node, int32_t mode);
// 1.13c D2CMP+0x11E0
extern "C" uint32_t d2cmp_11e0(uint8_t* m, uint32_t list, uint32_t extra, uint32_t flags, int32_t mode,
                               const D2CmpGuestEnv* e) {
    uint32_t node = e->alloc(m, extra + 0x24, kTagCelEntry, -2, flags | 8);
    if (node) { wr(m, node, 0); wr(m, node + 4, 0); }
    if (mode != 0) d2cmp_1740(m, 0, list, node, mode);
    return node;
}
