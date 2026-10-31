// 1.13c D2CMP+0x1A50 (stdcall list, extra, flags; ESI = mode): allocate an AUCELLIST of extra+0x38
// bytes (Storm flags | 8), construct it (0x1BA0), and if mode != 0 link it into list (0x19A0, where 0).
#include "d2cmp_guest.h"
using namespace d2cmpg;
extern "C" uint32_t d2cmp_1ba0(uint8_t* m, uint32_t p);
extern "C" void d2cmp_19a0(uint8_t* m, uint32_t list, uint32_t node, int32_t mode, uint32_t where);
// 1.13c D2CMP+0x1A50
extern "C" uint32_t d2cmp_1a50(uint8_t* m, uint32_t list, uint32_t extra, uint32_t flags, int32_t mode,
                               const D2CmpGuestEnv* e) {
    uint32_t node = e->alloc(m, extra + 0x38, kTagCelList, -2, flags | 8);
    if (node) node = d2cmp_1ba0(m, node);
    if (mode != 0) d2cmp_19a0(m, list, node, mode, 0);
    return node;
}
