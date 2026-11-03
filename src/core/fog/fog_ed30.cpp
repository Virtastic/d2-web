// 1.13c Fog+0xED30 : uint32 __stdcall(self, key), ret 8 -- look the record up with 0xD9E0(self, key, &cs) (it hands back the
// lock it took in cs), read the dword at record+0x8c (0 when absent), then LeaveCriticalSection(cs) when one was taken.
#include "fog_env.h"
extern "C" uint32_t fog_ed30(const FogEnv* e, uint32_t self, uint32_t key) {
    uint32_t cs = 0;
    uint32_t rec = fe_call(e, 0xD9E0, 0, 0, 0, 0, 0, self, key, (uint32_t)(uintptr_t)&cs);
    uint32_t res = 0;
    if (rec) res = fe_r32(rec + 0x8c);
    if (cs) fe_leave(e, cs);
    return res;
}
