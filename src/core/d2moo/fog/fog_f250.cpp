// 1.13c Fog+0xF250: uint __stdcall(const uint* pKey), ret 4, EDI = srv -- ban a QServer client by address id. key = *pKey; under srv+0x28 (helper 0x11690,
// EDX 0x56D) the chain srv[8][key & srv[3]] (id at +0x18, next at +0x1C) is searched. A hit whose 0xCEB0 (EAX = srv, stack node[6]) check is zero logs
// 0x190A0(0x6FF77040, 0x6FF78F18, node[0], 0xCDA6(key)) and calls 0xE910 (EAX = srv, stack node[0], file 0x6FF78FAC, line 0x580). Returns 1.
#include "fog_env.h"
extern "C" uint32_t fog_f250(const FogEnv* e, uint32_t edi, uint32_t pKey) {
    uint32_t key = fe_r32(pKey);
    uint32_t cs = edi + 0x28;
    uint32_t idx = fe_r32(edi + 0xC) & key;
    fe_call(e, 0x11690, 0, cs, 0x56D);
    fe_enter(e, cs);
    uint32_t n = fe_r32(fe_r32(edi + 0x20) + idx * 4);
    while (n && fe_r32(n + 0x18) != key) n = fe_r32(n + 0x1C);
    fe_leave(e, cs);
    if (n && fe_call(e, 0xCEB0, edi, 0, 0, 0, 0, fe_r32(n + 0x18)) == 0) {
        uint32_t s = fe_call(e, 0xCDA6, 0, 0, 0, 0, 0, key);
        fe_call(e, 0x190A0, 0, 0, 0, 0, 0, 0x6FF77040, 0x6FF78F18, fe_r32(n), s);
        fe_call(e, 0xE910, edi, 0, 0, 0, 0, fe_r32(n), 0x6FF78FAC, 0x580);
    }
    return 1;
}
