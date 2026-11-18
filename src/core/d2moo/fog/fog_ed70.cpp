// 1.13c Fog+0xED70 : uint16 __stdcall(self, key), ret 8 -- look the record up with 0xD9E0(self, key, &cs) (it hands back the
// lock it took in cs), read ntohs of the word at record+0x7a (0 when absent), then LeaveCriticalSection(cs) when one was taken.
#include "fog_env.h"
extern "C" uint32_t fog_ed70(const FogEnv* e, uint32_t self, uint32_t key) {
    uint32_t cs = 0;
    uint32_t rec = fe_call(e, 0xD9E0, 0, 0, 0, 0, 0, self, key, (uint32_t)(uintptr_t)&cs);
    uint32_t res = 0;
    if (rec) res = fe_call(e, 0xCDBE, 0, 0, 0, 0, 0, fe_r16(rec + 0x7a)) & 0xFFFF;   // Ordinal_15 (ntohs) of the word at +0x7a
    if (cs) fe_leave(e, cs);
    return res;
}
