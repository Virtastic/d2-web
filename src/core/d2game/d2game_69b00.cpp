// 1.13c D2Game+0x69B00 -- scaled stat value of a unit/item combination.
// EAX = item (0: the unit's inventory item #A54A([unit+0x60])), ECX = game (0 is fatal, line 0x963), EDX = unit,
// stack s0 = int* out, s1 = multiplier (ret 8). *out = #A7F0(unit); an item whose kind (#AA66) is 0x26 zeroes it. Then
//   *out += (((s1 - 1) * rec[0x19C] + rec[0x198] + #A80E(unit, item, game, 0) + #A802(unit, 0x77, 0)) * *out) / 100   (32-bit imul, signed /100)
// with rec = #A928(game); a player unit (type 0) whose class (+4) is in [0, dt[0xBC8]) then adds dt[0xBC4][class * 0xC4 + 0x3C].
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game69B00Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*unitValue)(uint32_t unit);                                    // #A7F0
    uint32_t (*invItem)(uint32_t inv);                                       // #A54A
    uint32_t (*itemKind)(uint32_t item);                                     // #AA66
    uint32_t (*calcA)(uint32_t unit, uint32_t item, uint32_t game, uint32_t z);   // #A80E
    uint32_t (*calcB)(uint32_t unit, uint32_t stat, uint32_t z);             // #A802
    uint32_t (*record)(uint32_t game);                                       // #A928 (ECX = game)
    uint32_t (*dataTables)();                                                // *(*0xF829C)
};

static inline uint32_t r32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void w32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

// 1.13c D2Game+0x69B00
void D2Game_69B00(uint32_t item, uint32_t game, uint32_t unit, uint32_t out, uint32_t mul, const D2Game69B00Env& e)
{
    w32(out, e.unitValue(unit));
    if (!game) { e.fatal(0x963); return; }
    if (!item) item = e.invItem(r32(unit + 0x60));
    if (item && e.itemKind(item) == 0x26) w32(out, 0);
    uint32_t sum = e.calcA(unit, item, game, 0);
    sum += e.calcB(unit, 0x77, 0);
    const uint32_t rec = e.record(game);
    int32_t base = (int32_t)r32(out);
    int32_t t = (int32_t)(((mul - 1) * r32(rec + 0x19c) + r32(rec + 0x198) + sum) * (uint32_t)base);
    // retail: edx = hi32(signed t * 0x51EB851F) >> 5 (arithmetic); quotient = edx + (edx >>> 31), i.e. t / 100 truncated toward zero
    const int32_t hi = (int32_t)(((int64_t)t * 0x51eb851f) >> 32) >> 5;
    const int32_t v = base + hi + (int32_t)((uint32_t)hi >> 31);
    w32(out, (uint32_t)v);
    if (unit && r32(unit) == 0) {
        const int32_t cls = (int32_t)r32(unit + 4);
        if (cls >= 0) {
            const uint32_t dt = e.dataTables();
            if (cls < (int32_t)r32(dt + 0xbc8)) {
                const uint32_t cr = r32(dt + 0xbc4) + (uint32_t)cls * 0xc4;
                if (cr) w32(out, r32(cr + 0x3c) + (uint32_t)v);
            }
        }
    }
}
