// Drop one entry from an id-keyed list: EAX = game, ECX = id, EBX = passed through to the release callee; stack list (head at +0,
// count at +4), flag; ret 8. The entry (+4 id, +0x14 next) is unlinked; a missing id returns at once. A seven-dword request {id, 0...}
// goes to the broadcast walker with EBX = the module's per-unit callback. The game's unit hash (+0x1320, bucket id & 0x7F, chain
// +0xE4, id +0xC) is searched for the entry's id: with the flag the unit is released (ECX = game, EAX = unit), else bit 31 of its
// +0xC4 word is cleared. The entry goes back to the game's pool (+0x1C) and the list count drops by one (a negative count is
// fatal). A rule over E (E::h = the callee call, E::blk = the stack block the walker is handed, E::rd32/wr32 = memory).
#ifndef D2GAME_RULE_97920
#define D2GAME_RULE_97920
#include <cstdint>

namespace g97920 {
template <class E> void Remove(E& e, uint32_t game, uint32_t id, uint32_t ebx, uint32_t list, uint32_t flag)
{
    uint32_t prev = 0, n = e.rd32(list);
    while (n && e.rd32(n + 4) != id) { prev = n; n = e.rd32(n + 0x14); }
    if (!n) return;
    if (prev) e.wr32(prev + 0x14, e.rd32(n + 0x14)); else e.wr32(list, e.rd32(n + 0x14));
    uint32_t req[7] = { id, 0, 0, 0, 0, 0, 0 };
    e.h(0xDFC10, 0, 0, 0, 0xB2096810u, 0, e.stk(req));   // EBX = the module callback (logged as a module address)
    e.blk(0xDFC10, req, 7);
    const uint32_t key = e.rd32(n + 4);
    uint32_t u = e.rd32(game + 0x1320 + (key & 0x7F) * 4);
    while (u && e.rd32(u + 0xC) != key) u = e.rd32(u + 0xE4);
    if (u) {
        if (flag) e.h(0x977E0, u, game, 0, ebx);
        else e.wr32(u + 0xC4, e.rd32(u + 0xC4) & 0x7FFFFFFFu);
    }
    e.h(0xA2EC, 0, e.rd32(game + 0x1C), n, 0, 0, 0xB20FBA38u, 0x25C, 0);   // pool free (file string, line)
    e.wr32(list + 4, e.rd32(list + 4) - 1);
}
}
#endif
