// 1.13c Fog+0xEA50: void __stdcall(self, key, a, b), ret 0x10 -- close and log a connection by key. idx = self[3] & key; under the self+0x28 lock
// (helper 0x11690, EDX 0x826) the chain self[0x20][idx] is walked (key at +0x18, next at +0x1C). A found node runs 0xE550 (EAX = node[0], ESI = self);
// a miss logs through 0x190A0 (cdecl 0x6FF77040, format 0x6FF78DE0, the string 0xCDA6(key)). Both then log 0x190A0(0x6FF77040, 0x6FF78CF4,
// 0xCDA6(key), a, b).
#include "fog_env.h"
extern "C" void fog_ea50(const FogEnv* e, uint32_t self, uint32_t key, uint32_t a, uint32_t b) {
    uint32_t cs = self + 0x28;
    uint32_t idx = fe_r32(self + 0xC) & key;
    fe_call(e, 0x11690, 0, cs, 0x826);
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
    fe_call(e, 0x190A0, 0, 0, 0, 0, 0, 0x6FF77040, 0x6FF78CF4, s, a, b);
}
