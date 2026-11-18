// 1.13c Fog+0xEC10: void __stdcall(self, key, a, b), ret 0x10 -- gated by self[0xBD0]. idx = self[3] & key; the lock is self[0x1C] + idx * 0x18
// (helper 0x11690, EDX 0x7A7); the chain self[0x14][idx] is walked (key at +0, next at +4). A miss logs 0x190A0(0x6FF77040, 0x6FF78E40, key, a, b)
// and returns. A hit: v = node[0x18], 0x190A0(0x6FF77040, 0x6FF78E08, 0xCDA6(v), a, b), 0xE550 (EAX = key, ESI = self), then 0xDF00 (ESI = self,
// stack v, a, b).
#include "fog_env.h"
extern "C" void fog_ec10(const FogEnv* e, uint32_t self, uint32_t key, uint32_t a, uint32_t b) {
    if (fe_r32(self + 0xBD0) == 0) return;
    uint32_t idx = fe_r32(self + 0xC) & key;
    uint32_t cs = fe_r32(self + 0x1C) + idx * 0x18;
    fe_call(e, 0x11690, 0, cs, 0x7A7);
    fe_enter(e, cs);
    uint32_t n = fe_r32(fe_r32(self + 0x14) + idx * 4);
    while (n && fe_r32(n) != key) n = fe_r32(n + 4);
    fe_leave(e, cs);
    if (!n) { fe_call(e, 0x190A0, 0, 0, 0, 0, 0, 0x6FF77040, 0x6FF78E40, key, a, b); return; }
    uint32_t v = fe_r32(n + 0x18);
    uint32_t s = fe_call(e, 0xCDA6, 0, 0, 0, 0, 0, v);
    fe_call(e, 0x190A0, 0, 0, 0, 0, 0, 0x6FF77040, 0x6FF78E08, s, a, b);
    fe_call(e, 0xE550, key, 0, 0, 0, self);
    fe_call(e, 0xDF00, 0, 0, 0, 0, self, v, a, b);
}
