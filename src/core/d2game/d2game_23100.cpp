// 1.13c D2Game+0x23100 -- unique-mod row eligibility (callee of the 0x23980 and 0x23AE0 rolls). Stack args
// (unit, entry, force), ret 0xC. entry: +4 u16 level cap, +6 enabled byte, +9 kind byte, +0xA two s16 requirement
// ids. Zero when the row is disabled, when force is 0 and the cap is >= 100, when 0x2790(EAX = unit, EDI = id)
// answers non-zero for a leading positive id, or when the kind test fails: kind 1 needs 0x1560(unit class, 4),
// kind 2 refuses MonStats rows whose +0xC byte hits gdwBitMasks[1] and any 0x15D0(class, 0x11) hit, kind 3
// needs 0x1560(unit class, 2). A null unit passes class -1. Callees are seams in retail register order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game23100Env {
    uint32_t (*sub2790)(uint32_t unit, int32_t id);
    uint32_t (*sub1560)(int32_t cls, uint32_t mode);
    uint32_t (*sub15D0)(int32_t cls, uint32_t mode);
    uint32_t dataTables;   // *sgptDataTables
    uint32_t bitMask1;     // gdwBitMasks[1]
};
static inline uint32_t g23100_u32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
static inline int16_t g23100_s16(uint32_t a, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 2); return v; }
static inline uint8_t g23100_u8(uint32_t a, uint32_t o) { return *((const uint8_t*)(uintptr_t)a + o); }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_23100(uint32_t unit, uint32_t entry, int32_t force, const D2Game23100Env* e)
{
    if (g23100_u8(entry, 6) == 0 || (force == 0 && (uint16_t)g23100_s16(entry, 4) >= 100)) return 0;
    for (int i = 0; i < 2; ++i) {
        const int16_t id = g23100_s16(entry, 10 + 2 * i);
        if (id <= 0) break;
        if (e->sub2790(unit, id)) return 0;
    }
    const int32_t cls = unit ? (int32_t)g23100_u32(unit, 4) : -1;
    switch (g23100_u8(entry, 9)) {
    case 1: return e->sub1560(cls, 4) ? 1 : 0;
    case 2: {
        if (unit && cls >= 0 && cls < (int32_t)g23100_u32(e->dataTables, 0xA80)) {
            const uint32_t row = g23100_u32(e->dataTables, 0xA78) + (uint32_t)cls * 0x1A8;
            if (row && (g23100_u8(row, 0xC) & e->bitMask1)) return 0;
        }
        return e->sub15D0(cls, 0x11) ? 0 : 1;
    }
    case 3: return e->sub1560(cls, 2) ? 1 : 0;
    }
    return 1;
}
