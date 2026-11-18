// 1.13c D2Client+0x4C7C0: EAX = unit, stack p0, p1, p2 (ret 0xC). For a type-1 unit whose monstats base id (row+2) is 0x3F it runs
// (count = row short at +0x62 + 2*DAT_6FBCC390) iterations of a two-step LCG (x*0x6AC690C5 + hi, seed lo 0x53454953 (SEED_InitLowSeed ord 10614), hi = unit id u[3]),
// each iteration flipping the +-5 offsets by the low bit of each new seed word and calling 0xB0450 (ESI = unit; stack p0, p1+dx, p2+dy, 0, 1; ret 0x14).
#include <emscripten.h>
#include <cstdint>
struct D2c4c7c0Env {
    const uint8_t* rows;        // *(sgptDataTables) + 0xA78: monstats rows, stride 0x1A8
    int32_t count;              // *(sgptDataTables) + 0xA80
    uint8_t clvl;               // DAT_6FBCC390 (difficulty, indexes the short list at row+0x62)
    void (*b0450)(const int32_t* unit, uint32_t a, int32_t x, int32_t y, uint32_t c, uint32_t d);
};
extern "C" void d2_client_4c7c0(const int32_t* u, uint32_t p0, uint32_t p1, uint32_t p2, const D2c4c7c0Env* e) {
    if (!u || u[0] != 1) return;
    int32_t cls = u[1], base = cls;
    auto row = [&](int32_t c) -> const uint8_t* { return (c >= 0 && c < e->count) ? e->rows + (uint32_t)c * 0x1a8 : nullptr; };
    if (const uint8_t* r = row(cls)) base = (int16_t)(r[2] | r[3] << 8);
    if (base < 0 || base >= e->count || base != 0x3f) return;
    const uint8_t* r = row(cls);
    if (!r) return;
    int32_t n = (int16_t)(r[0x62 + e->clvl * 2] | r[0x63 + e->clvl * 2] << 8);
    uint32_t lo = 0x53454953u, hi = (uint32_t)u[3];
    int32_t dx = 5, dy = 5;
    for (; n > 0; --n) {
        uint64_t t = (uint64_t)lo * 0x6ac690c5u + hi;
        uint32_t l1 = (uint32_t)t, h1 = (uint32_t)(t >> 32);
        if (l1 & 1) dx = -dx;
        uint64_t t2 = (uint64_t)l1 * 0x6ac690c5u + h1;
        lo = (uint32_t)t2; hi = (uint32_t)(t2 >> 32);
        if (lo & 1) dy = -dy;
        e->b0450(u, p0, (int32_t)p1 + dx, (int32_t)p2 + dy, 0, 1);
    }
}
