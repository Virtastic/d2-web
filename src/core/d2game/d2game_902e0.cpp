// 1.13c D2Game+0x902E0 -- reset a record and give it a fresh handle.
// ESI = record, EDI = game (pool at +0x1C), stack (ret 4) = optional out object. Frees the record's lists (0x90170), clears
// byte +0x20 and dword +0x28, stores a new handle (0xA448(pool, 0)) at +4 and, when the out object exists, at +0x60.
#include "d2game_8d6_common.h"
struct D2Game902E0Env {
    void (*freeLists)(uint32_t game, uint32_t rec);        // 0x90170 (EAX = game, stack = record)
    uint32_t (*newHandle)(uint32_t pool, uint32_t zero);   // 0xA448
};
// 1.13c D2Game+0x902E0
void d2game_port_902e0(uint32_t rec, uint32_t game, uint32_t out, const D2Game902E0Env* env)
{
    env->freeLists(game, rec);
    d2g8::w8(rec, 0x20, 0);
    d2g8::w32(rec, 0x28, 0);
    const uint32_t h = env->newHandle(d2g8::r32(game, 0x1C), 0);
    d2g8::w32(rec, 4, h);
    if (out) d2g8::w32(out, 0x60, h);
}
