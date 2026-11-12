// 1.13c D2Client+0x5CC10: the voice slot (0x10C75C0, 16 slots of 0x20 bytes, slot[2] = its entry) sounding a Sounds row,
// the one whose entry started earliest. A slot holds an entry exactly while that entry plays (0x5D7A0 fills it,
// 0x880E0 empties it), so the slot's entry is the playing entry of that row.
#include "play_sound_queue.h"

namespace d2client_playsnd {
Entry* slot5cc10(int32_t id) {
    Entry* best = nullptr;
    for (int32_t i = 0; i < kPool; ++i) {
        Entry* e = &g_pool[i];
        if (e->used && e->state == 1 && e->playing && e->id == id && (!best || e->start < best->start)) best = e;
    }
    return best;
}
}
