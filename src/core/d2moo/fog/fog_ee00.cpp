// 1.13c Fog+0xEE00: uint __stdcall(srv, key, p3, p4), ret 0x10 -- srv null gives 0. When *(srv+0x86C) is 1 or 2 and the callback at srv+0xBCC is set,
// return callback(ECX = srv, EDX = p3, stack p4) (ret 4). Otherwise 0xE8C0 with ESI = srv, EBX = p3, EDI = p4 and the key on the stack (ret 4).
#include "fog_env.h"
extern "C" uint32_t fog_ee00(const FogEnv* e, uint32_t srv, uint32_t key, uint32_t p3, uint32_t p4) {
    if (!srv) return 0;
    uint32_t mode = fe_r32(srv + 0x86C);
    uint32_t cb = fe_r32(srv + 0xBCC);
    if ((mode == 1 || mode == 2) && cb) return fe_call(e, cb & 0xFFFFFF, 0, srv, p3, 0, 0, p4);
    return fe_call(e, 0xE8C0, 0, 0, 0, p3, srv, key);
}
