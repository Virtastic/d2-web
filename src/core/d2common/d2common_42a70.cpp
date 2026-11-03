// 1.13c D2Common+0x42A70: stamp a level-preset entry's footprint onto the two tile grids of a DRLG context.
// ECX = ctx, EDX = x0, stack y0, idx, flag, extra (ret 0x10). The pointer at ctx+0x14 names a sub-struct: grid A cells at sub+0x2C with row
// offsets at sub+0x30, grid C cells at sub+0x04 with row offsets at sub+0x08 (retail 4-byte pointers, wasm32 host addresses). The three slot
// globals (PTR_FUN_6fdeb258 / 244 / 250) are static initialisers of tiny mask ops, so they run natively here:
//   0x36BD0 slot A: *cell &= ~mask      0x36C10 slot B: *cell |= mask      0x36C20 slot C: *cell = mask
#include <cstdint>
#include <cstddef>
#include <cstring>
#include "D2DataTbls.h"

int32_t D2Common_42920(void* pCtx, int32_t id, const void* pRow, int32_t* (*alloc)());   // d2common_batch_7ecc0.cpp
#define D2C_W __attribute__((weak))
extern "C" { D2C_W int32_t* (*g_d2c_alloc10045)(); }   // Ordinal_10045, the node allocator of 0x42920 (flag == -1 only)

namespace {
inline uint32_t r32(const void* p, int o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline uint32_t* cellAt(const void* sub, int cells, int offs, int32_t x, int32_t y)
{
    const int32_t* ro = (const int32_t*)(uintptr_t)r32(sub, offs);
    return (uint32_t*)(uintptr_t)r32(sub, cells) + (ro[y] + x);
}
inline void slotA(uint32_t* c, uint32_t m) { *c &= ~m; }
inline void slotB(uint32_t* c, uint32_t m) { *c |= m; }
inline void slotC(uint32_t* c, uint32_t m) { *c = m; }
}

// 1.13c D2Common+0x42A70
extern "C" void D2Common_42A70(void* ctx, int32_t x0, int32_t y0, int32_t idx, int32_t flag, int32_t extra)
{
    if (!sgptDataTables || !sgptDataTables->pLvlPrestTxt) return;
    const bool in = idx >= 0 && idx < sgptDataTables->nLvlPrestTxtRecordCount;
    // out of range: retail reads the row at address 0x28 and faults; ours is a zero row
    const uint8_t* row = in ? (const uint8_t*)sgptDataTables->pLvlPrestTxt + (std::size_t)idx * 0x1B0 : nullptr;
    const int32_t w = row ? (int32_t)r32(row, 0x28) : 0, h = row ? (int32_t)r32(row, 0x2C) : 0;
    if (flag == -1) {
        if (!row || !g_d2c_alloc10045) return;
        flag = D2Common_42920(ctx, idx, row, g_d2c_alloc10045);
    }
    const void* sub = (const void*)(uintptr_t)r32(ctx, 0x14);
    const int32_t yEnd = ((h + (h >> 31 & 7)) >> 3) + y0, xEnd = ((w + (w >> 31 & 7)) >> 3) + x0;
    const bool wide = (idx > 3 && idx < 0x10) || (idx > 0x16b && idx < 0x178);
    for (int32_t y = y0; y < yEnd; ++y)
        for (int32_t x = x0; x < xEnd; ++x) {
            uint32_t* a = cellAt(sub, 0x2C, 0x30, x, y);
            slotA(a, 0xF0000);
            slotB(a, ((uint32_t)flag << 16) | 0x200);
            if (extra != 0 && wide) slotB(a, 1);
            slotC(cellAt(sub, 0x04, 0x08, x, y), 0);
        }
    slotC(cellAt(sub, 0x04, 0x08, x0, y0), (uint32_t)idx);
}
