// Gold drop at a unit: the amount is dropped in piles of at most 2,000,000,000. Per pile the drop point is
// the unit's coordinates, nudged by (+2, +3) when 0x1280 finds a room there; D2Common (0xAE98) searches a free
// spot, whose room becomes the room of the next pile. The 0x84-byte drop block (type 4, flags 0x0A) goes
// to the item creator (0x11490); the pile gets flag 0x10 and its gold stat (14), zero when a ground
// gold item's cap is below the pile, and is placed at the drop point (0xE990). Retail is 32-bit: block
// fields are byte offsets and the callees are seams in retail argument order.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
inline void wr32(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }
inline void wr16(uint8_t* p, uint32_t o, uint16_t v) { std::memcpy(p + o, &v, 2); }
inline uint32_t u(const void* p) { return (uint32_t)(uintptr_t)p; }
}

struct D2G12B90Env {
    int32_t (*itemIndexFromCode)(uint32_t code, uint32_t* outIndex);                      // #10450 0xAA42
    void* (*unitRoom)(void* unit);                                                        // #10331 0xA478
    void (*unitCoords)(void* unit, uint32_t* outXY);                                      // #10141 0xA544
    void* (*roomAt)(void* roomEax, uint32_t xEbx, uint32_t yEdi);                          // 0x1280
    void* (*freeSpot)(void* room, uint32_t* inXY, uint32_t* outXY, int32_t a, int32_t b, int32_t c, int32_t d); // 0xAE98
    void* (*createItem)(void* game, uint8_t* drop, int32_t zero);                         // 0x11490
    void (*setItemFlag)(void* item, uint32_t flag, int32_t on);                           // 0xA65E
    int32_t (*goldCap)(void* item);                                                       // 0xA45A
    void (*setStat)(void* item, int32_t stat, int32_t value, int32_t layer);              // 0xA472
    void (*placeItem)(uint32_t eaxY, void* ecxItem, void* ebxRoom, void* game, void* unit, uint32_t x); // 0xE990
};

// 1.13c D2Game+0x12B90
extern "C" void d2g_12b90(void* game, void* unit, uint32_t amount, const D2G12B90Env* env) {
    uint32_t index = 0;
    if (!env->itemIndexFromCode(0x20646C67, &index)) return;   // 'gld '
    void* room = env->unitRoom(unit);
    uint32_t done = 0;
    const uint32_t total = amount;
    if (!total) return;
    do {
        const uint32_t pile = amount > 2000000000u ? 2000000000u : amount;
        amount -= pile;
        done += pile;
        uint32_t xy[2] = { 0, 0 }, in[2];
        env->unitCoords(unit, xy);
        if (env->roomAt(room, xy[0] + 2, xy[1] + 3)) { in[0] = xy[0] + 2; in[1] = xy[1] + 3; } else { in[0] = xy[0]; in[1] = xy[1]; }
        room = env->freeSpot(room, in, xy, 1, 0x3E01, 0x801, 1);
        if (!room) continue;
        uint8_t d[0x84] = {};
        wr32(d, 0x00, u(unit));
        wr16(d, 0x2A, rd16(game, 0x78));
        wr32(d, 0x14, index);
        wr16(d, 0x28, 1);
        wr32(d, 0x0C, 1);
        wr32(d, 0x04, 0);
        wr32(d, 0x08, u(game));
        wr32(d, 0x18, 4);
        wr32(d, 0x30, 2);
        wr32(d, 0x80, 0x0A);
        void* item = env->createItem(game, d, 0);
        if (!item) continue;
        env->setItemFlag(item, 0x10, 1);
        int32_t value = (int32_t)pile;
        if (value < 0) value = 0;
        else if (rd32(item, 0) == 0 && value > env->goldCap(item)) value = 0;
        env->setStat(item, 14, value, 0);
        env->placeItem(in[1], item, room, game, unit, in[0]);
    } while (done < total);
}
