// Item drop in a room at a level-scaled quality: the room's level (and the game's difficulty and seed word)
// pick an item level, one less when above 1; 0x2FA30 picks the item index (negative: nothing). The drop
// spot is searched from the caller's coordinates; the 0x84-byte drop block (flags 0x40) goes to the item
// creator (0x11490). Registers: EAX = coordinates pair, ECX = room, EBX = game. Retail is 32-bit; block
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

struct D2G11FB0Env {
    uint32_t (*roomLevel)(void* room);                                                    // #10630 0xA556
    int32_t (*levelItemLevel)(uint32_t level, uint32_t difficulty, uint32_t seedWord);    // 0xADA8
    int32_t (*pickItem)(uint32_t ecx, void* game, void* roomField, int32_t ilvl, uint32_t p2); // 0xFA30, ecx = p1
    void* (*dropSpot)(void* room, uint32_t* outXY, int32_t one, const uint32_t* inXY);    // 0xE930, inXY in ESI
    int32_t (*createItem)(void* game, const uint8_t* drop, int32_t zero);                 // 0x11490
};

// 1.13c D2Game+0x11FB0
extern "C" int32_t d2g_11fb0(const uint32_t* xy, void* room, void* game, uint32_t p1, uint32_t p2, uint32_t p3,
                             const D2G11FB0Env* env) {
    int32_t ilvl = env->levelItemLevel(env->roomLevel(room), ((const uint8_t*)game)[0x6D], rd32(game, 0x70));
    if (ilvl > 1) --ilvl;
    void* field = room ? (uint8_t*)room + 0x6C : nullptr;
    int32_t item = env->pickItem(p1, game, field, ilvl, p2);
    if (item < 0) return 0;
    uint32_t in[2] = { xy[0], xy[1] }, spot[2] = { 0, 0 };
    void* at = env->dropSpot(room, spot, 1, in);
    if (!at) return 0;
    uint8_t d[0x84] = {};
    wr32(d, 0x00, p3);
    wr32(d, 0x04, u(field));
    wr16(d, 0x2A, rd16(game, 0x78));
    wr32(d, 0x24, u(at));
    wr32(d, 0x14, (uint32_t)item);
    wr32(d, 0x1C, spot[0]);
    wr32(d, 0x08, u(game));
    wr32(d, 0x18, 3);
    wr32(d, 0x20, spot[1]);
    wr16(d, 0x28, 1);
    wr32(d, 0x30, 0);
    wr32(d, 0x0C, (uint32_t)ilvl);
    wr32(d, 0x80, 0x40);
    return env->createItem(game, d, 0);
}
