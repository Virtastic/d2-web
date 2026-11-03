// D2Game+0x4E390 -- copy one 9-byte tier row into a monster's data block.
// ecx = unit, eax = tier (signed, clamped to <= 10), edi = seed owner (seed u64 at +0x20), stack (setB, roll) ret 8.
// The row table (retail 0x6FD2D8C0, 9 bytes per tier) is passed in; retail data stays out of the tree.
// Each byte goes to unit->data only when unit != 0, unit->type == 1 and data != 0.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game4E390Env { const uint8_t* table; };

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
static inline uint32_t monData(uint32_t u) { return (u != 0 && rd32(u) == 1) ? rd32(u + 0x14) : 0; }
static inline void put8(uint32_t u, uint32_t off, uint8_t v) { const uint32_t d = monData(u); if (d) *(uint8_t*)(uintptr_t)(d + off) = v; }

// 1.13c D2Game+0x4E390
extern "C" void d2game_4e390(uint32_t unit, int32_t tier, uint32_t seedOwner,
                                                       uint32_t setB, uint32_t roll, const D2Game4E390Env* env)
{
    if (tier > 10) tier = 10;
    const uint8_t* row = env->table + tier * 9;
    put8(unit, 0x4, row[0]);
    put8(unit, 0x5, row[2]);
    put8(unit, 0xc, row[3]);
    put8(unit, 0xd, row[4]);
    put8(unit, 0x6, row[5]);
    put8(unit, 0x7, row[6]);
    put8(unit, 0x8, row[7]);
    if (setB) put8(unit, 0xb, row[1]);
    if (roll) {
        const uint64_t s = (uint64_t)rd32(seedOwner + 0x20) * 0x6ac690c5u + rd32(seedOwner + 0x24);
        wr32(seedOwner + 0x20, (uint32_t)s);
        wr32(seedOwner + 0x24, (uint32_t)(s >> 32));
        const uint8_t b = (uint8_t)(s & 3);
        put8(unit, 0xf, b);
        put8(unit, 0x10, b);
        const uint32_t d = monData(unit);
        if (d) { const uint32_t link = rd32(d + 0x28); if (link) { wr32(link + 0x14, 1); wr32(link + 0x18, 0); } }
        return;
    }
    put8(unit, 0x9, row[8]);
}
