// Item drop at a unit: the drop request is a zeroed 0x84-byte block on the stack, filled from the unit
// (EBX), its free drop spot and the caller's item index and arguments, then handed to the item creator
// (0x11490). A negative item index or no free spot answers 0. Retail is 32-bit, so every block field is
// a byte offset; the callees are seams in retail argument order.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
inline void wr32(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }
inline void wr16(uint8_t* p, uint32_t o, uint16_t v) { std::memcpy(p + o, &v, 2); }
inline uint32_t u(const void* p) { return (uint32_t)(uintptr_t)p; }
}

struct D2G11880Env {
    void (*unitCoords)(void* unit, uint32_t* outXY);                                    // #10141 0xA544
    void* (*unitRoom)(void* unit);                                                      // #10331 0xA478
    void* (*dropSpot)(void* room, uint32_t* outXY, int32_t one, const uint32_t* inXY);  // 0xE930, inXY in ESI
    uint32_t (*itemLevel)(void* unit, int32_t zero);                                    // 0xE5D0, unit in EAX
    int32_t (*createItem)(void* game, const uint8_t* drop, int32_t zero);               // 0x11490
};

// 1.13c D2Game+0x11880
extern "C" int32_t d2g_11880(void* unit, void* game, int32_t index, uint32_t a3, uint32_t a4, uint32_t flags,
                             const D2G11880Env* env) {
    if (index < 0) return 0;
    uint32_t xy[2] = { 0, 0 }, spot[2] = { 0, 0 };
    env->unitCoords(unit, xy);
    void* room = env->dropSpot(env->unitRoom(unit), spot, 1, xy);
    if (!room) return 0;
    uint8_t d[0x84] = {};
    wr16(d, 0x2A, rd16(game, 0x78));
    wr32(d, 0x14, (uint32_t)index);
    wr32(d, 0x24, u(room));
    wr32(d, 0x1C, spot[0]);
    wr32(d, 0x00, u(unit));
    wr32(d, 0x08, u(game));
    wr32(d, 0x18, 3);
    wr32(d, 0x20, spot[1]);
    wr16(d, 0x28, 1);
    wr32(d, 0x30, a3);
    wr32(d, 0x0C, env->itemLevel(unit, 0));
    wr32(d, 0x40, a4);
    uint32_t f = (unit && rd32(unit, 0) == 1 && rd32(unit, 4) == 0x187) ? 1 : 0;
    wr32(d, 0x80, f | flags);
    return env->createItem(game, d, 0);
}
