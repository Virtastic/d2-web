// 1.13c D2CMP+0x12E0 (stdcall 3 args): 0x1A50(a1, a2, a3) with ESI = 1 (link at the tail).
#include "d2cmp_guest.h"
extern "C" uint32_t d2cmp_1a50(uint8_t* m, uint32_t list, uint32_t extra, uint32_t flags, int32_t mode,
                               const D2CmpGuestEnv* e);
// 1.13c D2CMP+0x12E0
extern "C" uint32_t d2cmp_12e0(uint8_t* m, uint32_t list, uint32_t extra, uint32_t flags, const D2CmpGuestEnv* e) {
    return d2cmp_1a50(m, list, extra, flags, 1, e);
}
