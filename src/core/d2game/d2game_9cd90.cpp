// Monster idle/trigger gate: EAX = target, ECX = game, EBX = owner (plain ret). A target whose state gate (0x15) is set idles
// for 3 and the gate passes; otherwise the nearby-object trigger (EAX = owner, ECX = target, EDI = game) or the minion leash
// (EAX = owner, ECX = target; stack game) passing does. Result 0 or 1. A rule over E: E::h(callee, eax, ecx, edx, ebx, esi, stack...)
// is the callee call, E::edi() logs the EDI argument of the trigger.
#ifndef D2GAME_RULE_9CD90
#define D2GAME_RULE_9CD90
#include <cstdint>

namespace g9cd90 {
template <class E> uint32_t Gate(E& e, uint32_t tgt, uint32_t game, uint32_t owner)
{
    if (e.h(0xA68E, 0, 0, 0, 0, 0, tgt, 0x15)) { e.h(0xF22C0, game, 0, 0, 0, tgt, 3); return 1; }
    const uint32_t t = e.h(0x9CCF0, owner, tgt);
    e.edi(0x9CCF0, game);
    if (t) return 1;
    return e.h(0x9CB30, owner, tgt, 0, 0, 0, game) != 0;
}
}
#endif
