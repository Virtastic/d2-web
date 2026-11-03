// 1.13c D2Game+0x78FA0: drop a player's carried items and gold. Registers: EAX = unit; one stack word, the game (ret 4). The cursor item and the 12 body slots
// are taken out of the inventory (0xA6A6, 0xA7AE), detached (0x77D90 for the cursor, 0x78D00 for a slot), re-queued (0x107E0) and placed on free ground near the
// unit (0xAE98 around the point 0x21280 accepts, then 0xE990); the gold stat is cleared and handed to 0x37F80. Addresses are 32-bit words as in the FN probes.
#include <cstdint>
#include <cstring>
struct D2G78FA0Seams {
    uint32_t (*cursorItem)(uint32_t inv);                                                     // 0xA6A6
    uint32_t (*bodyItem)(uint32_t inv, uint32_t slot);                                        // 0xA7AE
    void     (*detachCursor)(uint32_t unit, uint32_t item);                                   // 0x77D90 (ESI = unit, EBX = item)
    void     (*detachSlot)(uint32_t unit, uint32_t item, uint32_t game, uint32_t slot);       // 0x78D00 (EAX = unit, ECX = item; stack game, slot)
    uint32_t (*takeItem)(uint32_t game, uint32_t item, uint32_t one);                         // 0x107E0 (EAX = game; stack item, 1)
    void     (*getPos)(uint32_t unit, uint32_t* pos);                                         // 0xA544
    uint32_t (*getRoom)(uint32_t unit);                                                       // 0xA478
    uint32_t (*roomHasPoint)(uint32_t room, int32_t x, int32_t y);                            // 0x1280 (EAX = room, EBX = x, EDI = y)
    uint32_t (*findFree)(uint32_t room, uint32_t* pos, uint32_t* orig, uint32_t a, uint32_t b, uint32_t c, uint32_t d); // 0xAE98
    void     (*placeItem)(uint32_t y, uint32_t item, uint32_t room, uint32_t game, uint32_t unit, uint32_t x);    // 0xE990 (EAX y, ECX item, EBX room)
    int32_t  (*getStat)(uint32_t unit, uint32_t stat, uint32_t layer);                        // 0xA42A
    int32_t  (*quantityCap)(uint32_t unit);                                                   // 0xA45A
    void     (*setStat)(uint32_t unit, uint32_t stat, int32_t value, uint32_t layer);         // 0xA472
    void     (*giveGold)(uint32_t game, uint32_t unitId, uint32_t unit, int32_t gold);        // 0x37F80 (EAX = game, ECX = unit id; stack unit, gold)
    void     (*fatal)(uint32_t line);                                                         // 0xA2DA, 0xA2C2, 0x22A3C(-1): never returns in retail
};
static inline uint32_t r32_78fa0(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
// 1.13c D2Game+0x78FA0
extern "C" void d2game_78fa0(uint32_t unit, uint32_t game, const D2G78FA0Seams* s) {
    const uint32_t inv = r32_78fa0(unit, 0x60);
    uint32_t orig[2] = { 0, 0 }, pos[2] = { 0, 0 };   // retail's stack slots persist across iterations
    for (int32_t slot = -1; slot < 13; ++slot) {
        const uint32_t item = slot == -1 ? s->cursorItem(inv) : s->bodyItem(inv, (uint32_t)slot);
        if (!item) continue;
        if (slot == -1) s->detachCursor(unit, item);
        else if (r32_78fa0(item, 0x10) == 1) s->detachSlot(unit, item, game, (uint32_t)slot);
        else { s->fatal(0x2a7); return; }
        const uint32_t taken = s->takeItem(game, item, 1);
        s->getPos(unit, orig);
        const uint32_t room = s->getRoom(unit);
        const int32_t x2 = (int32_t)orig[0] + 2, y3 = (int32_t)orig[1] + 3;
        if (s->roomHasPoint(room, x2, y3)) { pos[0] = (uint32_t)x2; pos[1] = (uint32_t)y3; } else { pos[0] = orig[0]; pos[1] = orig[1]; }
        const uint32_t dst = s->findFree(room, pos, orig, 1, 0x3e01, 0x801, 1);
        s->placeItem(pos[1], taken, dst, game, unit, pos[0]);
    }
    const int32_t gold = s->getStat(unit, 0xe, 0);
    if (r32_78fa0(unit, 0) == 0) s->quantityCap(unit);
    s->setStat(unit, 0xe, 0, 0);
    s->giveGold(game, r32_78fa0(unit, 0xc), unit, gold);
}
