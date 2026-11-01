// 1.13c D2Game+0x8FAF0 -- remove a viewer's entries from an NPC's vendor record.
// EAX = npc unit (class +4, null -> -1), ECX = viewer unit (id +0xC, null -> -1), one stack argument (ret 4) = game (pool at +0x1C).
// Record from 0x8F5C0. Entry list at +8 (only when +0xC is non-zero; entry: value +0, key +4, next +8): the entry whose key is the
// viewer id is unlinked, its value (if any) released by 0x8F470 then 0xAD96, and the entry freed (0xA2EC, line 0x48F).
// The chain at +0x18 (node: id +0, next +8): the node whose id matches is unlinked and freed (line 0x4A7).
#include "d2game_8d6_common.h"
struct D2Game8FAF0Env {
    uint32_t (*getRecord)(uint32_t game, uint32_t classId);   // 0x8F5C0 (ECX = game, ESI = class)
    void     (*sub8F470)(uint32_t value, uint32_t game);      // 0x8F470 (EAX = value, stack game)
    void     (*sub2AD96)(uint32_t value);                     // 0xAD96 (stdcall 1)
    void     (*freeBlock)(uint32_t pool, uint32_t block, uint32_t line);   // 0xA2EC (ECX = pool, EDX = block, stack file/line/0)
};
// 1.13c D2Game+0x8FAF0
void d2game_port_8faf0(uint32_t npc, uint32_t viewer, uint32_t game, const D2Game8FAF0Env* env)
{
    const uint32_t cls = npc ? d2g8::r32(npc, 4) : 0xFFFFFFFFu;
    const uint32_t rec = env->getRecord(game, cls);
    if (!rec) return;
    const uint32_t id = viewer ? d2g8::r32(viewer, 0xC) : 0xFFFFFFFFu;
    if (d2g8::r32(rec, 0xC)) {
        uint32_t prev = 0, n = d2g8::r32(rec, 8);
        for (; n; prev = n, n = d2g8::r32(n, 8)) if (d2g8::r32(n, 4) == id) break;
        if (n) {
            if (prev) d2g8::w32(prev, 8, d2g8::r32(n, 8)); else d2g8::w32(rec, 8, d2g8::r32(n, 8));
            const uint32_t value = d2g8::r32(n, 0);
            if (value) {
                env->sub8F470(value, game);
                env->sub2AD96(d2g8::r32(n, 0));
                d2g8::w32(n, 0, 0);
            }
            env->freeBlock(d2g8::r32(game, 0x1C), n, 0x48F);
        }
    }
    uint32_t prev = 0, n = d2g8::r32(rec, 0x18);
    for (; n; prev = n, n = d2g8::r32(n, 8)) if (d2g8::r32(n, 0) == id) break;
    if (!n) return;
    if (prev) d2g8::w32(prev, 8, d2g8::r32(n, 8)); else d2g8::w32(rec, 0x18, d2g8::r32(n, 8));
    env->freeBlock(d2g8::r32(game, 0x1C), n, 0x4A7);
}
