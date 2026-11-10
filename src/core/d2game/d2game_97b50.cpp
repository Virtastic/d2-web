// D2Game+0x97B50: remove a pet by GUID and tell the clients. ECX = game, EDX = player; stack: pet guid, flag (ret 8). The player's data (0xA4C0, fatal when
// missing) holds the pet table at +0x44 (none: return). The pet type comes from 0x96600 (EAX = player, EBX = guid). Type 0 with the flag set first calls
// 0x977E0 (EAX = flag, ECX = game, EDX = guid, EBX = guid); a negative type returns. A type below the data tables' pet-type count (+0xBF0) and a list entry
// (table head + type * 12, non-null) go to 0x97920 (EAX = game, ECX = guid; stack entry, flag), then the living-player walker 0xDFC10 gets a 7-dword request
// {guid, 0...} with EBX = the module callback 0x96810. A rule over E (E::h = the callee call, E::blk = the stack block, E::rd32 = memory).
#ifndef D2GAME_RULE_97B50
#define D2GAME_RULE_97B50
#include <cstdint>

namespace g97b50 {
template <class E> void Remove(E& e, uint32_t tabs, uint32_t game, uint32_t player, uint32_t guid, uint32_t a4)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, player);
    if (!pd) return;   // fatal in retail
    const uint32_t pets = e.rd32(pd + 0x44);
    if (!pets) return;
    const int32_t type = (int32_t)e.h(0x96600, player, 0, 0, guid);
    if (type == 0) { if (a4) e.h(0x977E0, a4, game, 0, guid); }
    else if (type < 0) return;
    if (type >= (int32_t)e.rd32(tabs + 0xBF0)) return;
    const uint32_t lst = e.rd32(pets) + (uint32_t)type * 12u;
    if (!lst) return;
    e.h(0x97920, game, guid, 0, 0, 0, lst, a4);
    uint32_t req[7] = { guid, 0, 0, 0, 0, 0, 0 };
    e.h(0xDFC10, 0, 0, 0, 0xB2096810u, 0, e.stk(req));
    e.blk(0xDFC10, req, 7);
}
}
#endif
