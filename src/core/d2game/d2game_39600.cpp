// 1.13c D2Game+0x39600 -- monster-mode hit callback: stores the row's level bonus (row +0x78, else the 0x1B10 per-level bonus) in the event record
// fastcall: EDX = unit, stack word 0 unused, stack word 1 = the event record (ret 8)
// Returns EAX at the ret: 0 (the entry value) on a null unit or row, the unit id on the two range exits, else the stored bonus.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f39600(NE& e, uint32_t dt, uint32_t unit, uint32_t rec)
{
    if (!unit) return 0;
    const int32_t id = (int32_t)e.rd32(unit + 4);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xb6c)) return (uint32_t)id;
    const uint32_t row = e.rd32(dt + 0xb64) + (uint32_t)id * 0x1a4u;
    if (!row) return 0;
    const uint32_t a = e.imp(0xB26A, unit);       // Ordinal 10240
    const uint32_t b = e.imp(0xB0FC, unit);       // Ordinal 11029
    int32_t v = (int32_t)e.rd32(row + 0x78);
    if (v < 1) v = (int32_t)e.h(0x1B10, a, 0, b);
    e.wr32(rec + 0x44, (uint32_t)v);
    e.wr32(rec + 0x60, 0x60);
    return (uint32_t)v;
}
}
