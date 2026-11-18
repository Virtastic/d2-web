// 1.13c Fog+0xE8C0: uint __stdcall(key) with ESI = srv, EBX = buf, EDI = len (ret 4) -- send buf to the client node found by 0xE4E0(srv, key):
// srv null, node absent or node+0xC != 0 returns 0. When *srv != 0 the send goes through 0x20180 (ECX = srv, stack ebx, edi) and its result is
// returned; otherwise Ordinal_19(node[4], buf, len, 0) (thunk 0xCD34): -1 logs Ordinal_111 (thunk 0xCD3A) and returns 0, anything else
// returns 1.
#include "fog_env.h"
extern "C" uint32_t fog_e8c0(const FogEnv* e, uint32_t esi, uint32_t ebx, uint32_t edi, uint32_t key) {
    if (!esi) return 0;
    uint32_t node = fe_call(e, 0xE4E0, 0, 0, 0, 0, 0, esi, key);
    if (!node) return 0;
    if (fe_r32(node + 0xC) != 0) return 0;
    if (fe_r32(esi) != 0) return fe_call(e, 0x20180, 0, esi, 0, 0, 0, ebx, edi);
    int32_t r = (int32_t)fe_call(e, 0xCD34, 0, 0, 0, 0, 0, fe_r32(node + 0x10), ebx, edi, 0);
    if (r == -1) { fe_call(e, 0xCD3A); return 0; }
    return 1;
}
