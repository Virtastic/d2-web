// 1.13c D2Game+0x13C20 -- weighted drop-table roll (stdcall: item, client). Rows of the D2Common table at
// tables+0xC18 (count +0xC1C, stride 0x1B8) qualify by drop level (+0x22, >= 100 needs item format >= 100),
// the signed level cap (+0x30 <= #10086), the item id (+0x28 == class record +0x80) and, for row word +0x2C
// == 0x1D, client+0x80 bit 0. client+0x40 - 1 forces a row. Else the weights (+0x34, 0 -> 1) feed one roll
// on the item's seed (#10411). The pick runs #10536, #10977(item, 0x10, 0), #10996(4, 0, item, 0, 0, 0) and
// answers 1; no table, no candidate or a roll past the list answers 0. Retail pointers are 32-bit.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game13C20Env {
    const uint8_t* tables;                         // the structure behind sgptDataTables
    uint32_t (*ordinal10695)(uint32_t classId);    // item class record (address)
    uint32_t (*ordinal10718)(uint32_t item);
    int32_t  (*ordinal10086)(uint32_t item);
    uint32_t (*ordinal10411)(uint32_t item);       // the item's seed pair (address)
    void (*ordinal10536)(uint32_t item, uint32_t row);
    void (*ordinal10977)(uint32_t item, uint32_t a, uint32_t b);
    void (*ordinal10996)(uint32_t a, uint32_t b, uint32_t item, uint32_t c, uint32_t d, uint32_t e);
};
static inline uint32_t g13c_rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
static inline uint32_t g13c_rd32(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static inline uint16_t g13c_rd16(const uint8_t* p, uint32_t o) { uint16_t v; std::memcpy(&v, p + o, 2); return v; }
extern "C" uint32_t d2_game_13C20(uint32_t item, uint32_t client, const D2Game13C20Env* env)
{
    int32_t chosen = -1;
    int32_t pairs[0x200];                          // (row, weight) per candidate
    int32_t n = 0; uint32_t total = 0;
    const uint32_t rec = env->ordinal10695(item ? g13c_rd32(item, 4) : 0xFFFFFFFFu);
    if (rec == 0) return 0;
    const uint32_t fmt = env->ordinal10718(item);
    const uint32_t recId = g13c_rd32(rec, 0x80);
    const int32_t level = env->ordinal10086(item);
    const int32_t rows = (int32_t)g13c_rd32(env->tables, 0xC1C);
    const uint8_t* table = (const uint8_t*)(uintptr_t)g13c_rd32(env->tables, 0xC18);
    if (rows <= 0) return 0;
    for (int32_t i = 0; i < rows; ++i) {
        const uint8_t* row = table + (size_t)i * 0x1B8;
        if (g13c_rd16(row, 0x22) >= 100 && (uint16_t)fmt < 100) continue;
        if ((int32_t)(int16_t)g13c_rd16(row, 0x30) > level) continue;
        if (recId != g13c_rd32(row, 0x28)) continue;
        if (g13c_rd16(row, 0x2C) == 0x1D && !(*(const uint8_t*)(uintptr_t)(client + 0x80) & 1)) continue;
        const uint32_t pick = g13c_rd32(client, 0x40);
        if (pick != 0 && pick - 1 == (uint32_t)i) chosen = i;
        uint32_t w = g13c_rd32(row, 0x34);
        pairs[n * 2] = i; pairs[n * 2 + 1] = w ? (int32_t)w : 1;
        total += (uint32_t)pairs[n * 2 + 1];
        ++n;
    }
    if (chosen < 0) {
        if (total == 0) return 0;
        uint32_t roll = 0;
        if ((int32_t)total > 0) {
            uint8_t* seed = (uint8_t*)(uintptr_t)env->ordinal10411(item);
            uint32_t lo, hi; std::memcpy(&lo, seed, 4); std::memcpy(&hi, seed + 4, 4);
            const uint64_t p = (uint64_t)lo * 0x6AC690C5u + hi;
            std::memcpy(seed, &p, 8);
            roll = (total & (total - 1)) ? (uint32_t)p % total : (total - 1) & (uint32_t)p;
        }
        int32_t k = 0;
        if (n <= 0) return 0;
        while (pairs[k * 2 + 1] <= (int32_t)roll) {
            roll -= (uint32_t)pairs[k * 2 + 1];
            if (++k >= n) return 0;
        }
        chosen = pairs[k * 2];
    }
    env->ordinal10536(item, (uint32_t)chosen);
    env->ordinal10977(item, 0x10, 0);
    env->ordinal10996(4, 0, item, 0, 0, 0);
    return 1;
}
