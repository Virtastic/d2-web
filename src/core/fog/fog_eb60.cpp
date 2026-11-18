// 1.13c Fog+0xEB60: void __stdcall(self, key, a, b), ret 0x10 -- as 0xEA50 but gated by self[0xBD0] (zero returns at once), lock EDX 0x7D3, the final
// log uses format 0x6FF78DA8 and the function ends with 0xDF00 (ESI = self, stack key, a, b) instead of returning.
#include "fog_env.h"
extern "C" void fog_eb60(const FogEnv* e, uint32_t self, uint32_t key, uint32_t a, uint32_t b) {
    if (fe_r32(self + 0xBD0) == 0) return;
    uint32_t cs = self + 0x28;
    uint32_t idx = fe_r32(self + 0xC) & key;
    fe_call(e, 0x11690, 0, cs, 0x7D3);
    fe_enter(e, cs);
    uint32_t n = fe_r32(fe_r32(self + 0x20) + idx * 4);
    while (n && fe_r32(n + 0x18) != key) n = fe_r32(n + 0x1C);
    fe_leave(e, cs);
    if (n) fe_call(e, 0xE550, fe_r32(n), 0, 0, 0, self);
    else {
        uint32_t s = fe_call(e, 0xCDA6, 0, 0, 0, 0, 0, key);
        fe_call(e, 0x190A0, 0, 0, 0, 0, 0, 0x6FF77040, 0x6FF78DE0, s);
    }
    uint32_t s = fe_call(e, 0xCDA6, 0, 0, 0, 0, 0, key);
    fe_call(e, 0x190A0, 0, 0, 0, 0, 0, 0x6FF77040, 0x6FF78DA8, s, a, b);
    fe_call(e, 0xDF00, 0, 0, 0, 0, self, key, a, b);
}
