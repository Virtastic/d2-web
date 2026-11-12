// D2Game+0xD2840: hand an item that could not be taken back to the ground and play the refusal sound on the unit.
// ESI = item, EDI = unit, one stack word = the sound id (ret 4). Written from the 1.13c disassembly; the play path runs
// the same rule as ITEMMODE_ReturnItemToGround in d2moo/src/D2Game/ITEMS/ItemMode.cpp.
// 0xA44E(0xA478(item), type, id) drops the item's room entry (type 6, id -1 for a null item); 0xA92E(item, 0xFF) clears its
// page; the item's flag word +0xC4 gains 0x1000; 0xA4A2(item, 3) puts it in the on-ground mode. A null unit assert-exits
// (line 0x558, not modelled). The unit's sound word +0x6E takes the id, +0x70 the unit itself, 0xA4DE(unit) queues the
// update and the unit's flag word +0xC4 gains 0x400. EAX is what 0xA4DE leaves.
#include "d2game_regcall.h"
// 1.13c D2Game+0xD2840
extern "C" uint32_t d2game_d2840(uint32_t item, uint32_t unit, uint32_t sound, const D2GameRegCall* e)
{
    using namespace d2grc;
    const uint32_t type = item ? r32(item) : 6, id = item ? r32(item + 0xC) : 0xFFFFFFFFu;
    stk(e, 0xA44E, { stk(e, 0xA478, { item }), type, id });
    stk(e, 0xA92E, { item, 0xFF });
    if (item) w32(item + 0xC4, r32(item + 0xC4) | 0x1000);
    stk(e, 0xA4A2, { item, 3 });
    if (!unit) return 0;
    w16(unit + 0x6E, sound);
    w32(unit + 0x70, unit);
    const uint32_t r = stk(e, 0xA4DE, { unit });
    w32(unit + 0xC4, r32(unit + 0xC4) | 0x400);
    return r;
}
