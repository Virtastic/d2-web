// 1.13c D2Client+0x5D7A0: start a queued entry. Unless the submit asked for its own row (+0x38 bit 0) the entry takes
// the group variant 0x5EE70 picks. It starts unless the same row already sounds (0x5CC10) from an entry that is not
// killed, started at most one tick before this one and has at least this one's priority. A start shifts the asked
// row's pick history (+0x68 = +0x64, +0x64 = the row started). The file load / D2Sound voice steps that follow are the
// browser mixer's.
#include <cstring>
#include "play_sound_queue.h"
#include "d2client_5dfd0.h"

namespace d2client_playsnd {
bool start5d7a0(Entry* e) {
    uint8_t* asked = rec(e->id);
    if (!(e->flags & 1)) e->id = d2_client_5ee70(d2client_sounds::records(), d2client_sounds::recordCount(), e->id, seedRand);
    const Entry* s = slot5cc10(e->id);
    if (s && !s->killed && e->start - s->start <= 1 && s->prio >= e->prio) return false;
    if (asked) { memcpy(asked + 0x68, asked + 0x64, 4); memcpy(asked + 0x64, &e->id, 4); }
    e->playing = 1;
    return true;
}
}
