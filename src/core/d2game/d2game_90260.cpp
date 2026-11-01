// 1.13c D2Game+0x90260 -- attach a 20-byte proxy node for a unit to its class record.
// EAX = game (pool at +0x1C), ECX = unit (null -> class -1; class at +4), stack (ret 8) = arg1 (owner id) and arg2.
// Node: +0 arg1, +8 arg2, +0xC 1, +0x10 old head of the record's chain at +0x14. No record: run the living-player callback and remove arg1.
#include "d2game_8d6_common.h"
struct D2Game90260Env {
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line);   // 0xA2E6
    uint32_t (*getRecord)(uint32_t game, uint32_t cls);               // 0x8F5C0
    void (*iteratePlayers)(uint32_t arg);                             // 0xDFC10 with callback 0x6FC2E7E0
    void (*removeUnit)(uint32_t game, uint32_t unit);                 // 0xE20A0
};
// 1.13c D2Game+0x90260
void d2game_port_90260(uint32_t game, uint32_t unit, uint32_t arg1, uint32_t arg2, const D2Game90260Env* env)
{
    const uint32_t node = env->alloc(d2g8::r32(game, 0x1C), 0x14, 0x85);
    const uint32_t cls = unit ? d2g8::r32(unit, 4) : 0xFFFFFFFFu;
    const uint32_t rec = env->getRecord(game, cls);
    if (rec) {
        d2g8::w32(node, 0x10, d2g8::r32(rec, 0x14));
        d2g8::w32(rec, 0x14, node);
        d2g8::w32(node, 0, arg1);
        d2g8::w32(node, 0xC, 1);
        d2g8::w32(node, 8, arg2);
    } else {
        env->iteratePlayers(arg1);
        env->removeUnit(game, arg1);
    }
}
