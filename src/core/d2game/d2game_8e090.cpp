// 1.13c D2Game+0x8E090 -- unit/client assertion (fatal 0x7D2), zero the position of ECX, then 0x8D3D0 action 1
//. EAX = unit, ECX = object to zero, EDX = expected client, [esp+4] = argument (ret 4).
#include "d2game_8d6_common.h"
struct D2Game8E090Env {
    uint32_t (*getPlayerData)(uint32_t unit);                                       // 0xA4C0
    void     (*ord10882)(uint32_t p, uint32_t v);                                   // 0xA8F2 SetX
    void     (*ord11118)(uint32_t p, uint32_t v);                                   // 0xA964 SetY
    void     (*sub8D3D0)(uint32_t client, uint32_t id, uint32_t arg, uint32_t flag);
};
// 1.13c D2Game+0x8E090
void d2game_port_8e090(uint32_t unit, uint32_t p, uint32_t client, uint32_t arg, const D2Game8E090Env* env)
{
    uint32_t owner = 0;
    if (unit && d2g8::r32(unit, 0) == 0) owner = d2g8::r32(env->getPlayerData(unit), 0x9C);
    if (owner != client) d2g8::fatal(0x7D2);
    env->ord10882(p, 0);
    env->ord11118(p, 0);
    env->sub8D3D0(client, 1, arg, 0);
}
