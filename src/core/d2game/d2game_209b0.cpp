// 1.13c D2Game+0x209B0: SUNITINACTIVE_CompressUnitIfNeeded. ESI = unit, one stack word = game (ret 4).
// Unit layout (retail 32-bit): +0x00 type, +0x04 classId, +0x10 animMode, +0xC4 flags, +0xC8 flagEx, +0xDC event chain
// (node +0x00 type byte, +0x24 next). Type 1 (monster) tail-jumps to 0x40760; the rest is the switch below.
// Every callee is an env seam so the FN probe can record its arguments in retail order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game209B0Env {
    uint32_t (*getRoom)(uint32_t unit);                         // Ordinal 10331, unit in ESI
    uint32_t (*getLevelId)(uint32_t room);                      // Ordinal 10826
    int32_t  (*getSaveMonsters)(uint32_t levelId);              // Ordinal 10929
    void     (*compressMonster)(uint32_t unit, uint32_t game);  // 0x40760, unit in EAX
    int32_t  (*checkPlayerBody)(uint32_t unit, uint32_t state); // Ordinal 10494 (unit, 7)
    uint32_t (*getObjectRecord)(uint32_t classId);              // Ordinal 10688
    uint32_t (*checkSparklyChest)(uint32_t unit);               // 0xDF6C0, unit in EAX
    void     (*deleteEvent)(uint32_t game, uint32_t event);     // 0x8E6F0, event in EAX, game in EDI
    void     (*compressInactiveUnit)(uint32_t unit, uint32_t game); // 0x404E0, unit in EAX
    void     (*updatePath)(uint32_t unit);                      // Ordinal 10650
    void     (*removeUnit)(uint32_t game, uint32_t unit);       // 0xE20A0 (0x20A0 in the switch tail): ECX game, EDX unit
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline uint8_t  rd8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }

static void deleteEndAnimEvents(uint32_t game, uint32_t unit, const D2Game209B0Env* e)
{
    for (uint32_t ev = rd32(unit, 0xDC); ev != 0;) {
        const uint8_t type = rd8(ev, 0);
        const uint32_t next = rd32(ev, 0x24);   // read before the callee runs
        if (type == 1) e->deleteEvent(game, ev);
        ev = next;
    }
}

// the 0x3B/0x3C and body exits: flag the unit deleted, compress, update the path (tail call)
static void finishDeleted(uint32_t game, uint32_t unit, const D2Game209B0Env* e)
{
    deleteEndAnimEvents(game, unit, e);
    const uint32_t fx = rd32(unit, 0xC8) | 0x100;
    std::memcpy((void*)(uintptr_t)(unit + 0xC8), &fx, 4);
    e->compressInactiveUnit(unit, game);
    e->updatePath(unit);
}

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_game_209B0(uint32_t game, uint32_t unit, const D2Game209B0Env* e)
{
    const uint32_t room = e->getRoom(unit);
    const uint32_t type = rd32(unit, 0);
    const uint32_t classId = rd32(unit, 4);
    if (type == 1) { e->compressMonster(unit, game); return; }
    bool compress = false;
    if (e->getSaveMonsters(e->getLevelId(room)) != 0 || (rd32(unit, 0xC4) & 0x2000000u) != 0) compress = true;
    switch (type) {
    case 0:
        if (e->checkPlayerBody(unit, 7) != 0) { finishDeleted(game, unit, e); return; }
        break;
    case 2: {
        if (classId == 0x3B || classId == 0x3C) { finishDeleted(game, unit, e); return; }
        const uint32_t rec = e->getObjectRecord(classId);
        if (rd8(rec, 0x173) == 0 || (e->checkSparklyChest(unit) & 2) != 0) compress = false;
        if (rd8(rec, 0x174) != 0 && rd32(unit, 0x10) != 0) compress = false;
        deleteEndAnimEvents(game, unit, e);
        break;
    }
    case 4:
    case 5:
        e->compressInactiveUnit(unit, game);        // the case 4/5 edge skips the compress flag test
        if (type != 4) e->removeUnit(game, unit);
        return;
    default:
        if (type != 4) e->removeUnit(game, unit);   // types 3 and > 5 skip the compress as well
        return;
    }
    if (compress) e->compressInactiveUnit(unit, game);
    e->removeUnit(game, unit);                      // type 0 / 2 here, never 4
}
