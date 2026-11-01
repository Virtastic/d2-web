// 1.13c D2Game+0x8dfd0 -- unit/client assertion then the 0x8D3D0 action 0x3.
// EAX = unit, ECX = expected client, stack = one argument (ret 4). Sibling of 0x8DE50 .. 0x8E030; the decompile hides ECX and the stack argument.
#include "d2game_8d6_common.h"
struct D2Game8DFD0Env {
    uint32_t (*getPlayerData)(uint32_t unit);                                              // #10920 (0xA4C0, one stack word)
    void     (*sub8D3D0)(uint32_t client, uint32_t id, uint32_t arg, uint32_t flag);        // 0x8D3D0 (client in EBX, three stack words)
};
// 1.13c D2Game+0x8dfd0
void d2game_port_8dfd0(uint32_t unit, uint32_t client, uint32_t arg, const D2Game8DFD0Env* env)
{
    uint32_t owner = 0;
    if (unit && d2g8::r32(unit, 0) == 0) owner = d2g8::r32(env->getPlayerData(unit), 0x9C);
    if (owner != client) d2g8::fatal(0x7e6);
    env->sub8D3D0(client, 0x3, arg, 0);
}
