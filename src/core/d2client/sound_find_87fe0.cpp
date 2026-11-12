// 1.13c D2Client+0x87FE0 (EAX = Sounds row, stack = entry to skip): a live entry of the row's Compound group. The first
// list entry whose row shares the group base (record +0x60), is not the skipped one and is not killed (+0x3D), when
// the row's Compound (+0x45, signed word) is negative or the entry started at most Compound ticks ago.
#include <cstring>
#include "play_sound_queue.h"

namespace d2client_playsnd {
Entry* find87fe0(int32_t id, const Entry* self) {
    const uint8_t* r = rec(id);
    if (!r) return nullptr;
    int32_t grp; memcpy(&grp, r + 0x60, 4);
    int16_t comp; memcpy(&comp, r + 0x45, 2);
    for (int32_t i = 0; i < g_listLen; ++i) {
        Entry* e = &g_pool[g_list[i]];
        const uint8_t* er = rec(e->id);
        int32_t eg = 0; if (er) memcpy(&eg, er + 0x60, 4);
        if (eg != grp || e == self || e->killed) continue;
        if (comp < 0 || g_now - e->start <= (uint32_t)(int32_t)comp) return e;
    }
    return nullptr;
}
}
