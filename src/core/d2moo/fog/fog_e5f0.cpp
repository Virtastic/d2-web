// 1.13c Fog+0xE5F0: FogQNode* __stdcall(p1, p2), ret 8, EAX = srv -- register a new QServer connection record. A 0x664-byte zeroed node comes from
// 0x1B560 (EAX 0x664, ECX 0, stack file 0x6FF78FAC, line 0x241; a NULL result is the fatal 0x10F30 path, not modelled). Under the srv+0x854 lock (helper 0x11690,
// EDX 0x24C) id = ++srv[1]. node[0] = id, [4]... [0x10] = p1, [0x18] = p2, [0x58] = 1, [0x70] = 2, [0x98..0xA4] = the 0x1C9E0 time, the node critical
// section at +0x2C is initialised (import). The node is pushed on the bucket (srv[3] & id) of srv[5] (next at +4) under the lock srv[7] + idx * 0x18 (EDX
// 0x27C) and on the bucket (srv[3] & p2) of srv[8] (next at +0x1C) under srv+0x28 (EDX 0x282). Last, under srv+0x854 again (EDX 0x289): 0xD110 (EAX = &p2,
// EBX = srv), node[2]++ and srv[0x880]++. Ordinal_11 (thunk 0xCDA6) runs on p2; the node is returned.
#include "fog_env.h"
extern "C" uint32_t fog_e5f0(const FogEnv* e, uint32_t srv, uint32_t p1, uint32_t p2) {
    const uint32_t FILE_ = 0x6FF78FAC;
    uint32_t node = fe_call(e, 0x1B560, 0x664, 0, 0, 0, 0, FILE_, 0x241);
    memset(fe_p(node), 0, 0x664);
    uint32_t cs = srv + 0x854;
    fe_call(e, 0x11690, 0, cs, 0x24C);
    fe_enter(e, cs);
    uint32_t id = fe_r32(srv + 4) + 1;
    fe_w32(srv + 4, id);
    fe_leave(e, cs);
    fe_w32(node + 0x10, p1); fe_w32(node, id); fe_w32(node + 0x18, p2);
    fe_w32(node + 0x24, 0); fe_w32(node + 0x28, 0); fe_w32(node + 0x74, 0);
    fe_w32(node + 0x58, 1); fe_w32(node + 0x70, 2);
    uint32_t t = fe_call(e, 0x1C9E0);
    fe_w32(node + 0x98, t); fe_w32(node + 0x9C, t); fe_w32(node + 0xA0, t); fe_w32(node + 0xA4, t);
    uint32_t idx = fe_r32(srv + 0xC) & id;
    uint32_t cs2 = fe_r32(srv + 0x1C) + idx * 0x18;
    fe_call(e, 0x11690, 0, cs2, 0x27C);
    fe_enter(e, cs2);
    fe_w32(node + 4, fe_r32(fe_r32(srv + 0x14) + idx * 4));
    fe_w32(fe_r32(srv + 0x14) + idx * 4, node);
    fe_leave(e, cs2);
    uint32_t cs3 = srv + 0x28;
    fe_call(e, 0x11690, 0, cs3, 0x282);
    fe_enter(e, cs3);
    uint32_t i2 = fe_r32(srv + 0xC) & p2;
    fe_w32(node + 0x1C, fe_r32(fe_r32(srv + 0x20) + i2 * 4));
    fe_w32(fe_r32(srv + 0x20) + i2 * 4, node);
    fe_leave(e, cs3);
    fe_call(e, 0x11690, 0, cs, 0x289);
    fe_enter(e, cs);
    uint32_t p2l = p2;
    fe_call(e, 0xD110, (uint32_t)(uintptr_t)&p2l, 0, 0, srv);
    fe_w32(node + 8, fe_r32(node + 8) + 1);
    fe_w32(srv + 0x880, fe_r32(srv + 0x880) + 1);
    fe_leave(e, cs);
    fe_call(e, 0xCDA6, 0, 0, 0, 0, 0, fe_r32(node + 0x18));
    return node;
}
