// 1.13c Storm+0x36E00 (ordinal 440, stdcall totalW, totalH, tileW, tileH): rebuild the STRANS row-offset table. The old table (0x55360) is freed
// (STRANS.CPP, 0x508) and cleared; the shifts for tileW (0x515E8) and tileH (0x515F0) are stored before each power-of-two test, a non-power returns 0.
// Then 0x515EC/0x515F4 := tileW/tileH, 0x515E0/0x515E4 := the rounded-up column/row counts (arithmetic shift), the table := SMemAlloc(rows*4,
// STRANS.CPP, 0x517, 0) with t[i] = i*cols. Returns 1.
#include <cstdint>

extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
extern "C" void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);
namespace storm_b36d40 { extern uint32_t g_trans_c; }   // 0x55360

// 0x515E0 cols, 0x515E4 rows, 0x515E8 shX, 0x515EC tileW, 0x515F0 shY, 0x515F4 tileH
extern "C" int32_t g_strans515e0[6] = {};

extern "C" uint32_t Storm13c_36E00(int32_t totalW, int32_t totalH, int32_t tileW, int32_t tileH) {
    static const char* kTrans = "..\\3rdParty\\STORM\\SOURCE\\STRANS.CPP";
    int32_t* g = g_strans515e0;
    uint32_t& tab = storm_b36d40::g_trans_c;
    if (tab != 0) { Storm_Sub1B960((void*)(uintptr_t)tab, kTrans, 0x508); tab = 0; }
    int32_t p = 1;
    g[2] = 0;
    if (1 < tileW) do { p *= 2; g[2]++; } while (p < tileW);
    if (p != tileW) return 0;
    p = 1;
    g[4] = 0;
    if (1 < tileH) do { p *= 2; g[4]++; } while (p < tileH);
    if (p != tileH) return 0;
    g[0] = (int32_t)((uint32_t)(1 << (g[2] & 0x1f)) - 1u + (uint32_t)totalW) >> (g[2] & 0x1f);
    g[3] = tileW;
    g[5] = tileH;
    g[1] = (int32_t)((uint32_t)(1 << (g[4] & 0x1f)) - 1u + (uint32_t)totalH) >> (g[4] & 0x1f);
    uint32_t* t = (uint32_t*)Storm_Sub1C580((uint32_t)g[1] * 4u, kTrans, 0x517, 0);
    tab = (uint32_t)(uintptr_t)t;
    uint32_t acc = 0;
    for (int32_t i = 0; i < g[1]; i++) { t[i] = acc; acc += (uint32_t)g[0]; }
    return 1;
}
