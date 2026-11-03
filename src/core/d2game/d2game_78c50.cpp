// 1.13c D2Game+0x78C50: spawn a quantity item (item stat 0xE) at a unit's position and tag it. Registers: EAX = unit, EBX = source (or 0), EDI = quantity
// (signed; nothing happens when <= 0); one stack word, the game (ret 4), is handed on in ESI to the item creator 0x11CA0. Addresses are 32-bit words as in the FN probes.
#include <cstdint>
#include <cstring>
struct D2G78C50Seams {
    void     (*getPos)(uint32_t unit, uint32_t* pos);                                              // 0xA544: pos[0], pos[1] = x, y
    uint32_t (*getRoom)(uint32_t unit);                                                           // 0xA478
    uint32_t (*createItem)(uint32_t* pos, uint32_t game, uint32_t room, uint32_t a, uint32_t b, uint32_t src); // 0x11CA0 (EAX = pos, ESI = game; stack room, 4, 1, EBX)
    int32_t  (*itemType)(uint32_t item);                                                          // 0xAA66 (must be 4)
    int32_t  (*quantityCap)(uint32_t item);                                                       // 0xA45A
    void     (*setStat)(uint32_t item, uint32_t stat, int32_t value, uint32_t layer);             // 0xA472
    void     (*setOwnerTag)(uint32_t item, uint32_t tag);                                         // 0xB054
    void     (*fatal)(uint32_t line);                                                             // 0xA2DA, 0xA2C2, 0x22A3C(-1): never returns in retail
};
// 1.13c D2Game+0x78C50
extern "C" void d2game_78c50(uint32_t unit, uint32_t ebx, int32_t edi, uint32_t game, const D2G78C50Seams* s) {
    if (edi <= 0) return;
    uint32_t pos[2] = { 0, 0 };
    s->getPos(unit, pos);
    const uint32_t room = s->getRoom(unit);
    const uint32_t item = s->createItem(pos, game, room, 4, 1, ebx);
    if (!item) return;
    if (s->itemType(item) != 4) { s->fatal(0x24b); return; }
    uint32_t first; std::memcpy(&first, (const void*)(uintptr_t)item, 4);
    int32_t value = edi;
    if (first == 0 && edi > s->quantityCap(item)) value = 0;
    s->setStat(item, 0xe, value, 0);
    uint32_t tag = 0xffffffffu;
    if (ebx) std::memcpy(&tag, (const void*)(uintptr_t)(ebx + 0xc), 4);
    s->setOwnerTag(item, tag);
}
