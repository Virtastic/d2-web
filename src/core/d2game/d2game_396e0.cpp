// D2Game+0x396E0 (MISSMODE_SrvHit55_Baalnferno): fastcall (ecx unused, edx = P2 {+4 row index}, stack word 0 = P3 stat-list pointer, ret 4).
// P3 null or *P3 != 0 returns 2. P2 null, an index outside [0, dt[0xB6C]) or a null row (row = dt[0xB64] + idx * 0x1A4) or row[0x4C] <= 0 returns 1
// (dt = *sgptDataTables, the word at 0x6FD1829C). Else n = Ordinal 10973(P3, 8, 0) (thunk 0xA42A); n <= 0 returns 1. Else x = 0x214D0(eax = clamp(row[0x4C], 1, 100),
// ecx = 100, edx = n), x = min(max(x, 1), n); Ordinal 10887(P3, 8, n - x, 0) (thunk 0xA472); returns 2.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game396E0Env {
    int32_t (*getStat)(uint32_t list, uint32_t stat, uint32_t layer);              // Ordinal 10973
    int32_t (*scale)(uint32_t pct, uint32_t hundred, uint32_t n);                  // 0x214D0 (eax, ecx, edx)
    void    (*setStat)(uint32_t list, uint32_t stat, int32_t value, uint32_t layer); // Ordinal 10887
    uint32_t dt;                                                                    // *sgptDataTables
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Game+0x396E0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_396E0(uint32_t p2, uint32_t p3, const D2Game396E0Env* e)
{
    if (!p3 || rd32(p3)) return 2;
    if (!p2) return 1;
    const int32_t idx = (int32_t)rd32(p2 + 4);
    if (idx < 0 || idx >= (int32_t)rd32(e->dt + 0xb6c)) return 1;
    const uint32_t row = (uint32_t)idx * 0x1a4u + rd32(e->dt + 0xb64);
    if (!row) return 1;
    const int32_t pct = (int32_t)rd32(row + 0x4c);
    if (pct <= 0) return 1;
    const int32_t n = e->getStat(p3, 8, 0);
    if (n <= 0) return 1;
    int32_t c = pct > 1 ? pct : 1;
    if (c >= 100) c = 100;
    int32_t x = e->scale((uint32_t)c, 100, (uint32_t)n);
    if (x <= 1) x = 1;
    if (x >= n) x = n;
    e->setStat(p3, 8, n - x, 0);
    return 2;
}
