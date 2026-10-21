// Quest 0x28 (Act-4-style) clear: if neither state bit 0 nor 1 is set and the unit's room/level
// type is 4, reset bits 0 and 0xD, then call 4A2D0 with the client slot (game+0x6d).
// Callees are seams in retail argument order.
#include <cstdint>
namespace d2game_batch58c90 {
using U = uint32_t; using P = void*;
struct Env {
    P (*playerData)(P unit);                       // Ordinal 10920 (quest ptrs at +0x10 + slot*4)
    int (*questGet)(U q, U quest, U flag);         // Ordinal 10174
    P (*getRoom)(P unit);                          // Ordinal 10331
    P (*getLevel)(P room);                         // Ordinal 10826
    int8_t (*levelType)(P level);                  // Ordinal 10459
    void (*questSet)(U q, U quest, U flag);        // Ordinal 10003
    void (*sub_4A2D0)(U slot);                     // 0x4A2D0 (slot in ecx/edx)
};
// 1.13c D2Game+0x58C90
void sub_58C90(const Env& e, P game, P unit) {
    U slot = *((uint8_t*)game + 0x6d);
    U q = ((U*)((uint8_t*)e.playerData(unit) + 0x10))[slot];
    if (e.questGet(q, 0x28, 0) == 1 || e.questGet(q, 0x28, 1) == 1) return;
    P room = e.getRoom(unit); if (!room) return;
    P lvl = e.getLevel(room); if (!lvl) return;
    if (e.levelType(lvl) != 4) return;
    e.questSet(q, 0x28, 0);
    e.questSet(q, 0x28, 0xd);
    if (unit && *(U*)unit == 0) e.playerData(unit);
    e.sub_4A2D0(slot);
}
}
