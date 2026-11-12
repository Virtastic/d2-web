// 1.13c D2Client+0x87C30 (group, unit, entry): the earliest-started playing entry (+0x30 == 1) of a Sounds group
// (record +0x60), the given entry excluded. The unit argument adds D2Sound #10053 on the voice (see 0x87BD0).
#include <cstring>
#include "play_sound_queue.h"

namespace d2client_playsnd {
Entry* inst87c30(int32_t group, const Entry* self) {
    Entry* best = nullptr;
    for (int32_t i = 0; i < g_listLen; ++i) {
        Entry* e = &g_pool[g_list[i]];
        const uint8_t* er = rec(e->id);
        int32_t eg = 0; if (er) memcpy(&eg, er + 0x60, 4);
        if (eg == group && e != self && e->state == 1 && (!best || e->start < best->start)) best = e;
    }
    return best;
}
}
