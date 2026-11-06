// 1.13c D2Common+0x81CA0 (stdcall, 4 args, ret 0x10): builds the logic block of a level and its scratch grid.
// level: +0x3C width, +0x40 height, +0x58 drlg (+0x1B4 points at a struct whose +0x478 is the ctx word; +0x1DC region counter), +0x64 out (block).
// Block (0x34 bytes, zeroed): +0 flags (bit 2 set after 0x81650), +4 region count, +0x30 region array (count * 0x30 bytes, zeroed).
// Scratch grid on the stack: cols = w + 1, rows = h + 1, cells zeroed, rowoff[i] = i * cols; grid struct {cells, rowoff, cols, rows, 0}.
// Request for 0x81BC0: {level, block + 8, p2, p4, p3, &grid, counter-at-entry, 0}; the callee advances req[6]; region count = req[6] - counter + 1.
#include <cstdint>
#include <cstring>
#include "d2common_81ca0.h"

namespace {
inline uint32_t R32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
inline void W32(uint32_t a, uint32_t v) { *(uint32_t*)(uintptr_t)a = v; }
}

extern "C" void d2c_81ca0_run(uint32_t level, uint32_t p2, uint32_t p3, uint32_t p4, const D2c81CA0Env* e)
{
	const uint32_t drlg = R32(level + 0x58);
	const uint32_t ctx = R32(R32(drlg + 0x1B4) + 0x478);
	const uint32_t blk = e->alloc(ctx, 0x34, 0xDF);
	memset((void*)(uintptr_t)blk, 0, 0x34);
	W32(level + 0x64, blk);
	e->c81650(level, p4);
	W32(blk, R32(blk) | 2);
	const int32_t cols = (int32_t)R32(level + 0x3C) + 1, rows = (int32_t)R32(level + 0x40) + 1;
	e->c36950(ctx, blk + 8, cols, rows);
	if (!R32(drlg + 0x1DC)) W32(drlg + 0x1DC, 1);
	const uint32_t cnt0 = R32(drlg + 0x1DC);
	uint32_t cells[0x401] = {}, rowoff[0x100] = {};
	for (int32_t i = 0, o = 0; i < rows && i < 0x100; ++i, o += cols) rowoff[i] = (uint32_t)o;
	const uint32_t grid[5] = { (uint32_t)(uintptr_t)cells, (uint32_t)(uintptr_t)rowoff, (uint32_t)cols, (uint32_t)rows, 0 };
	e->c814c0(level, grid);
	uint32_t req[8] = { level, blk + 8, p2, p4, p3, (uint32_t)(uintptr_t)grid, cnt0, 0 };
	e->c81bc0(req);
	const uint32_t cnt = req[6] - cnt0 + 1;
	W32(blk + 4, cnt);
	const uint32_t arr = e->alloc(ctx, cnt * 0x30, 0xE9);
	W32(blk + 0x30, arr);
	memset((void*)(uintptr_t)arr, 0, cnt * 0x30);
	W32(drlg + 0x1DC, R32(drlg + 0x1DC) + cnt);
	e->c81200(level, blk);
	e->c81a20(level);
	e->c81ad0(level);
}
