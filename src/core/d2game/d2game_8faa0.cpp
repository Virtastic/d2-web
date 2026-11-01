// 1.13c D2Game+0x8FAA0 -- look up a vendor-record entry for an NPC and a viewer unit.
// EAX = npc unit (class dword +4, null -> -1), ECX = game, EDI = viewer unit (id +0xC, null -> -1); no stack argument.
// The record comes from 0x8F5C0 (EAX = out slot, ECX = game, ESI = class); +0xC must be non-zero; the entry list starts at +8
// (entry: value +0, key +4, next +8) and the value of the entry whose key is the viewer id is returned, else 0.
#include "d2game_8d6_common.h"
struct D2Game8FAA0Env {
    uint32_t (*getRecord)(uint32_t game, uint32_t classId);   // 0x8F5C0 (ECX = game, ESI = class)
};
// 1.13c D2Game+0x8FAA0
uint32_t d2game_port_8faa0(uint32_t npc, uint32_t game, uint32_t viewer, const D2Game8FAA0Env* env)
{
    const uint32_t cls = npc ? d2g8::r32(npc, 4) : 0xFFFFFFFFu;
    const uint32_t rec = env->getRecord(game, cls);
    if (!rec || !d2g8::r32(rec, 0xC)) return 0;
    const uint32_t key = viewer ? d2g8::r32(viewer, 0xC) : 0xFFFFFFFFu;
    for (uint32_t n = d2g8::r32(rec, 8); n; n = d2g8::r32(n, 8)) if (d2g8::r32(n, 4) == key) return d2g8::r32(n, 0);
    return 0;
}
