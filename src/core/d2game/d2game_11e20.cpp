// Item drop in a room from a pick that is retried: the room's level (with the game's difficulty and seed
// word) gives an item level, one less when above 1; 0xF8F0 picks an item index, up to six times until the
// index is non-negative and 0xB1B6 accepts it. The drop point is the caller's coordinates, nudged by (+2, +3)
// when 0x1280 finds a room there; D2Common (0xAE98) searches the free spot whose room goes in the 0x84-byte
// drop block (flags 0x40) handed to the item creator (0x11490). Registers: ESI = game. Retail is 32-bit;
// block fields are byte offsets and the callees are seams in retail argument order.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
inline void wr32(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }
inline void wr16(uint8_t* p, uint32_t o, uint16_t v) { std::memcpy(p + o, &v, 2); }
inline uint32_t u(const void* p) { return (uint32_t)(uintptr_t)p; }
}

struct D2G11E20Env {
    uint32_t (*roomLevel)(void* room);                                                    // #10630 0xA556
    int32_t (*levelItemLevel)(uint32_t level, uint32_t difficulty, uint32_t seedWord);    // 0xADA8
    int32_t (*pickItem)(uint32_t ecx, void* game, void* roomField, int32_t ilvl, uint32_t p4); // 0xF8F0, ecx = p3
    int32_t (*itemUsable)(int32_t index);                                                 // 0xB1B6
    void* (*roomAt)(void* roomEax, uint32_t xEbx, uint32_t yEdi);                          // 0x1280
    void* (*freeSpot)(void* room, uint32_t* inXY, uint32_t* outXY, int32_t a, int32_t b, int32_t c, int32_t d); // 0xAE98
    int32_t (*createItem)(void* game, const uint8_t* drop, int32_t zero);                 // 0x11490
};

// 1.13c D2Game+0x11E20
extern "C" int32_t d2g_11e20(void* game, void* room, const uint32_t* xy, uint32_t p3, uint32_t p4, uint32_t p5,
                             const D2G11E20Env* env) {
    int32_t ilvl = env->levelItemLevel(env->roomLevel(room), ((const uint8_t*)game)[0x6D], rd32(game, 0x70));
    if (ilvl > 1) --ilvl;
    void* field = room ? (uint8_t*)room + 0x6C : nullptr;
    int32_t item;
    int tries = 0;
    do {
        item = env->pickItem(p3, game, field, ilvl, p4);
        if (item >= 0 && env->itemUsable(item)) break;
    } while (++tries < 6);
    if (item < 0) return 0;
    uint32_t org[2] = { xy[0], xy[1] }, in[2];
    if (env->roomAt(room, org[0] + 2, org[1] + 3)) { in[0] = org[0] + 2; in[1] = org[1] + 3; } else { in[0] = org[0]; in[1] = org[1]; }
    void* at = env->freeSpot(room, in, org, 1, 0x3E01, 0x801, 1);
    if (!at) return 0;
    uint8_t d[0x84] = {};
    wr32(d, 0x00, p5);
    wr32(d, 0x04, u(field));
    wr16(d, 0x2A, rd16(game, 0x78));
    wr32(d, 0x24, u(at));
    wr32(d, 0x14, (uint32_t)item);
    wr32(d, 0x1C, in[0]);
    wr32(d, 0x08, u(game));
    wr32(d, 0x18, 3);
    wr32(d, 0x20, in[1]);
    wr16(d, 0x28, 1);
    wr32(d, 0x30, 0);
    wr32(d, 0x0C, (uint32_t)ilvl);
    wr32(d, 0x80, 0x40);
    return env->createItem(game, d, 0);
}
