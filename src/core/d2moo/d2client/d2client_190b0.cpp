// 1.13c D2Client+0x190B0: walks the unit list of playerSelf's room: type-1 units with flag 0x3F -> 0x6A220(unit, 0)
#include "cl17.h"
extern "C" uint32_t d2c_190b0(uint32_t edi, const cl::Env& e) {
    uint32_t self = e.self;
    if (!self) return self;
    uint32_t room = e.H(0xc1f0, self);
    if (!room) return 0;
    uint32_t cnt = 0, arr = 0;
    e.H(0xc3dc, room, cl17::ptr32(&arr), cl17::ptr32(&cnt));
    for (uint32_t i = 0; i < cnt; ++i) {
        uint32_t esi = e.R(e.R(arr + i * 4) + 0x74);
        while (esi) {
            if (e.R(esi) == 1 && e.H(0xc1fc, esi, 0x3f)) e.H(0x6a220, 0, 0, 0, 0, 0, esi);
            esi = e.R(esi + 0xe8);
        }
    }
    return cnt;
}
