// 1.13c Fog+0xE550: void, plain ret, EAX = key, ESI = srv -- close a QServer remote connection. node = 0xE4E0(srv, key); no node or node[3] != 0
// returns. Otherwise node[3] = 1, r = Ordinal_3(node[4]) (closesocket, thunk 0xCD82), node[4] = -1; r != 0 logs through 0x190A0 (cdecl
// 0x6FF768EC, format 0x6FF78F70, Ordinal_111 error). node[2] is decremented (InterlockedDecrement); at zero the srv+0xBC8 callback (ECX = srv,
// stack file 0x6FF78FAC, 0x374), then 0xE1B0(EAX = srv, stack node[0]) and 0xE3B0 (EBX = node). r == -1 ends with Ordinal_111 (thunk 0xCD3A).
#include "fog_env.h"
extern "C" void fog_e550(const FogEnv* e, uint32_t key, uint32_t esi) {
    uint32_t node = fe_call(e, 0xE4E0, 0, 0, 0, 0, 0, esi, key);
    if (!node || fe_r32(node + 0xC) != 0) return;
    fe_w32(node + 0xC, 1);
    int32_t r = (int32_t)fe_call(e, 0xCD82, 0, 0, 0, 0, 0, fe_r32(node + 0x10));
    fe_w32(node + 0x10, 0xFFFFFFFF);
    if (r != 0) {
        uint32_t err = fe_call(e, 0xCD3A);
        fe_call(e, 0x190A0, 0, 0, 0, 0, 0, 0x6FF768EC, 0x6FF78F70, err);
    }
    uint32_t cnt = fe_r32(node + 8) - 1;
    fe_w32(node + 8, cnt);
    if (cnt == 0) {
        uint32_t cb = fe_r32(esi + 0xBC8);
        if (cb) fe_call(e, cb & 0xFFFFFF, 0, esi, fe_r32(node), 0, 0, 0x6FF78FAC, 0x374);
        fe_call(e, 0xE1B0, esi, 0, 0, 0, 0, fe_r32(node));
        fe_call(e, 0xE3B0, 0, 0, 0, node);
    }
    if (r == -1) fe_call(e, 0xCD3A);
}
