// Item drop at a unit with a rolled item id (the 1.10f D2GAME_DropItemAtUnit_6FC4FEC0 body, written from the 1.13c
// disassembly): the item level goes to *itemLevel; a unit drop code (+0xB8) fixes the id, else it is rolled, and a
// quality-4 drop re-rolls until the items.txt record has bit 0 of +0xDC (11 more rolls, then the forced roll). The drop
// spot is the free space next to the unit's coordinates; no spot answers 0. The 0x84-byte drop record is handed to the
// item creator and copied to *itemDrop.
#include "d2game_11a70.h"
#include <cstring>

namespace {
inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
inline void wr32(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }
inline void wr16(uint8_t* p, uint32_t o, uint16_t v) { std::memcpy(p + o, &v, 2); }
inline uint32_t u(const void* p) { return (uint32_t)(uintptr_t)p; }
}

// 1.13c D2Game+0x11A70
extern "C" int32_t d2g_11a70(void* game, void* unit, int32_t quality, uint32_t* itemLevel, uint8_t* itemDrop,
                             int32_t a6, int32_t a7, const D2G11A70Env* env) {
    if (!game) env->fatal(0x9C7);
    void* seed = (uint8_t*)unit + 0x20;
    const int32_t isMonster = (unit && rd32(unit, 0) == 1) ? 1 : 0;
    *itemLevel = env->itemLevel(unit, 0);
    int32_t id;
    const uint32_t code = rd32(unit, 0xB8);
    if (code) {
        id = env->idFromCode(code);
        if (id == -1) env->fatal(0x9EA);
    } else {
        id = env->rollId(game, seed, *itemLevel, a6, a7, isMonster);
        if (quality == 4) {
            int32_t tries = 0;
            const uint8_t* rec = (const uint8_t*)env->itemsRec(id);
            while (!rec || !(rec[0xDC] & 1)) {
                id = tries <= 10 ? env->rollId(game, seed, *itemLevel, a6, a7, isMonster)
                                 : env->rollIdForced(game, seed, *itemLevel, a6, a7);
                rec = (const uint8_t*)env->itemsRec(id);
                ++tries;
            }
        }
    }
    uint32_t xy[2] = { 0, 0 }, spot[2] = { 0, 0 };
    env->unitCoords(unit, xy);
    void* room = env->dropSpot(env->unitRoom(unit), spot, 1, xy);
    if (!room) return 0;
    uint8_t d[0x84] = {};
    wr32(d, 0x24, u(room));
    wr16(d, 0x2A, rd16(game, 0x78));
    wr32(d, 0x30, (uint32_t)quality);
    wr32(d, 0x00, u(unit));
    wr32(d, 0x1C, spot[0]);
    wr32(d, 0x20, spot[1]);
    wr32(d, 0x04, 0);
    wr32(d, 0x08, u(game));
    wr32(d, 0x14, (uint32_t)id);
    wr32(d, 0x18, 3);
    wr16(d, 0x28, 1);
    wr32(d, 0x0C, *itemLevel);
    const int32_t item = env->createItem(game, d, 0);
    if (itemDrop) std::memcpy(itemDrop, d, sizeof d);
    return item;
}
