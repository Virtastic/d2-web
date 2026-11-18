// 1.13c Fog+0xF6E0: uint __stdcall(self, arg), ret 8 -- *self != 0 returns 0x21050(EAX = self, stack arg) (ret 4). Otherwise the fatal tail: 0xFC20 (caller IP),
// 0xCD8E (stack dump), 0x10F30(6, file, 0x129, fmt, ip), 0x184A0, _exit(-1) (not capturable).
#include "fog_env.h"
extern "C" uint32_t fog_f6e0(const FogEnv* e, uint32_t self, uint32_t arg) {
    if (fe_r32(self) != 0) return fe_call(e, 0x21050, self, 0, 0, 0, 0, arg);
    uint32_t ip = fe_call(e, 0xFC20);
    fe_call(e, 0xCD8E);
    fe_call(e, 0x10F30, 0, 0, 0, 0, 0, 6, 0xB2000000u | 0x268EC, 0x129, 0xB2000000u | 0x2850C, ip);
    fe_call(e, 0x184A0);
    fe_call(e, 0x1BA9, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
    return 0;
}
