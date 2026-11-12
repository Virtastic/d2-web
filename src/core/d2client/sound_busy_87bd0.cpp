// 1.13c D2Client+0x87BD0: another playing entry (+0x30 == 1) of a Sounds row in the local voice range 0xB76..0x1230,
// the first in list order. The caller's unit (EBX) adds D2Sound #10053 on the entry's voice, which holds for a voice
// the mixer still sounds: an entry the browser reported ended is no longer playing, so the test is the state alone.
#include "play_sound_queue.h"

namespace d2client_playsnd {
Entry* busy87bd0(const Entry* self) {
    for (int32_t i = 0; i < g_listLen; ++i) {
        Entry* e = &g_pool[g_list[i]];
        if (e != self && e->state == 1 && e->id > 0xB75 && e->id < 0x1231) return e;
    }
    return nullptr;
}
}
