// 0x61880 (ECX = quest, EDX = event; plain ret): runs 0x340C0 (EBX = event, ESI = quest), then removes the event client's unit id (-1 if none) from the lists at
// quest data (+0x18) +0xB4 and +0x138 through 0x33830. Returns the second result.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x61880
uint32_t d2game_61880_q32(const Call& X, uint32_t quest, uint32_t ev) {
    X(0x340C0, 0, 0, 0, ev, quest);
    uint32_t cu = rd32(ev, 0xc);
    uint32_t id = cu ? rd32(cu, 0xc) : 0xffffffffu;
    uint32_t d = rd32(quest, 0x18);
    X(0x33830, id, d + 0xb4);
    return X(0x33830, id, d + 0x138);
}
