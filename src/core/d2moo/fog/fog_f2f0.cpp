// 1.13c Fog+0xF2F0: uint __stdcall(self, buf, len), ret 0xC -- for each bucket i < self[2]: lock self[7] + i * 0x18 (helper 0x11690, ECX = lock, EDX = 0x217),
// walk the chain self[5][i] (next +4); every node with node[3] == 0 gets the buffer: *self != 0 calls 0x20180 (EAX = node, ECX = self, stack buf, len), else
// Ordinal_19 send (thunk 0xCD34: socket node[4], buf, len, 0) and -1 logs Ordinal_111 (thunk 0xCD3A). Always returns 1.
#include "fog_env.h"
extern "C" uint32_t fog_f2f0(const FogEnv* e, uint32_t self, uint32_t buf, uint32_t len) {
    for (uint32_t i = 0; i < fe_r32(self + 8); i++) {
        uint32_t cs = fe_r32(self + 0x1c) + i * 0x18;
        fe_call(e, 0x11690, 0, cs, 0x217);
        fe_enter(e, cs);
        for (uint32_t n = fe_r32(fe_r32(self + 0x14) + i * 4); n; n = fe_r32(n + 4)) {
            if (fe_r32(n + 0xc) != 0) continue;
            if (fe_r32(self) != 0) fe_call(e, 0x20180, n, self, 0, 0, 0, buf, len);
            else if ((int32_t)fe_call(e, 0xCD34, 0, 0, 0, 0, 0, fe_r32(n + 0x10), buf, len, 0) == -1) fe_call(e, 0xCD3A);
        }
        fe_leave(e, fe_r32(self + 0x1c) + i * 0x18);
    }
    return 1;
}
