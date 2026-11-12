// 1.13c D2Common+0x42270 -- ECX = record, EDX = column, one stack argument = row. Through the record's +0x14 block: the row start table at +0x30 gives the row's first cell, the cell array at +0x2C holds flag dwords. Returns 1 when bit 10 of the cell at (row start + column) is clear, 0 when it is set.
#include <cstdint>
static inline uint32_t rd32(uint32_t a) { uint32_t v; __builtin_memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr32(uint32_t a, uint32_t v) { __builtin_memcpy((void*)(uintptr_t)a, &v, 4); }
uint32_t d2common_42270(uint32_t rec, uint32_t col, uint32_t row)
{
    const uint32_t blk = rd32(rec + 0x14);
    const uint32_t cell = rd32(rd32(blk + 0x30) + row * 4) + col;
    return ~(rd32(rd32(blk + 0x2c) + cell * 4) >> 10) & 1;
}
