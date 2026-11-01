// Item drop in a room at a level-scaled item level capped at 64: the room's level (with the game's
// difficulty and seed word) gives the item level, one less when above 1; 0x2FFC0 picks the item index.
// The drop point is the caller's coordinates, nudged by (+2, +3) when 0x1280 finds a room there; a free
// spot is searched through D2Common (0xAE98) and the 0x84-byte drop block goes to the item creator
// (0x11490), then is copied to the caller's buffer when one is given. EAX = coordinates pair. Retail is
// 32-bit; block fields are byte offsets and the callees are seams in retail argument order.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
inline void wr32(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }
inline void wr16(uint8_t* p, uint32_t o, uint16_t v) { std::memcpy(p + o, &v, 2); }
inline uint32_t u(const void* p) { return (uint32_t)(uintptr_t)p; }
}

struct D2G12230Env {
    uint32_t (*roomLevel)(void* room);                                                    // #10630 0xA556
    int32_t (*levelItemLevel)(uint32_t level, uint32_t difficulty, uint32_t seedWord);    // 0xADA8
    int32_t (*pickItem)(int32_t ilvlEdi, void* game, int32_t minusOne, int32_t z0, int32_t z1); // 0xFFC0, ilvl in EDI
    void* (*roomAt)(void* roomEax, uint32_t xEbx, uint32_t yEdi);                          // 0x1280
    void* (*freeSpot)(void* room, uint32_t* inXY, uint32_t* outXY, int32_t a, int32_t b, int32_t c, int32_t d); // 0xAE98
    int32_t (*createItem)(void* game, uint8_t* drop, int32_t zero);                       // 0x11490
};

// 1.13c D2Game+0x12230
extern "C" int32_t d2g_12230(const uint32_t* xy, void* game, void* room, uint8_t* out, const D2G12230Env* env) {
    int32_t ilvl = env->levelItemLevel(env->roomLevel(room), ((const uint8_t*)game)[0x6D], rd32(game, 0x70));
    if (ilvl > 1) --ilvl;
    if (ilvl >= 0x41) ilvl = 0x40;
    void* field = room ? (uint8_t*)room + 0x6C : nullptr;
    int32_t item = env->pickItem(ilvl, game, -1, 0, 0);
    uint32_t x = xy[0], y = xy[1];
    uint32_t in[2], spot[2] = { x, y };
    if (env->roomAt(room, x + 2, y + 3)) { in[0] = x + 2; in[1] = y + 3; } else { in[0] = x; in[1] = y; }
    void* at = env->freeSpot(room, in, spot, 1, 0x3E01, 0x801, 1);
    if (!at) return 0;
    uint8_t d[0x84] = {};
    wr16(d, 0x2A, rd16(game, 0x78));
    wr32(d, 0x14, (uint32_t)item);
    wr32(d, 0x1C, in[0]);
    wr32(d, 0x20, in[1]);
    wr32(d, 0x24, u(at));
    wr32(d, 0x00, 0);
    wr32(d, 0x04, u(field));
    wr32(d, 0x08, u(game));
    wr32(d, 0x18, 3);
    wr16(d, 0x28, 1);
    wr32(d, 0x30, 0);
    wr32(d, 0x0C, (uint32_t)ilvl);
    int32_t r = env->createItem(game, d, 0);
    if (out) std::memcpy(out, d, sizeof d);
    return r;
}
