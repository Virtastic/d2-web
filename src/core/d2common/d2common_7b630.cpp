// 1.13c D2Common+0x7B630 (fastcall, ECX = path info {start, target}, packed x | y << 16): A* driver over a stack search context.
// Context (0x32EC bytes, zero-filled where retail zeroes it): +0x000 and +0x200 two 128-slot tables, +0x400 pending list head,
// +0x404 node pool (up to 200 nodes of 0x38 bytes), +0x2FC4 pool count. Node: +0 position, +4 and +6 score words, +8 path length word.
#include <cstdint>
#include <cstring>
#include "d2common_7b630.h"

namespace {
inline uint32_t R32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
inline int16_t R16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }
inline void W16(uint32_t a, uint16_t v) { *(uint16_t*)(uintptr_t)a = v; }
}

extern "C" uint32_t d2c_7b630_run(uint32_t info, const D2c7B630Env* e)
{
	if (!e->check(info)) return 0;
	const uint32_t start = R32(info), target = R32(info + 4);
	alignas(8) uint8_t ctxBuf[0x32F0];
	memset(ctxBuf, 0, sizeof ctxBuf);
	const uint32_t ctx = (uint32_t)(uintptr_t)ctxBuf;
	uint32_t* count = (uint32_t*)(ctxBuf + 0x2FC4);
	uint32_t node = 0;
	if (*count != 200) { node = ctx + 0x404 + *count * 0x38; ++*count; }
	int32_t dx = (int32_t)(start & 0xFFFF) - (int32_t)(target & 0xFFFF); if (dx < 0) dx = -dx;
	int32_t dy = (int32_t)(start >> 16) - (int32_t)(target >> 16); if (dy < 0) dy = -dy;
	const int32_t h = dx < dy ? dx + dy * 2 : dy + dx * 2;
	W16(node + 6, (uint16_t)h); W16(node + 4, (uint16_t)h);
	*(uint32_t*)(uintptr_t)node = start;
	e->insert(ctx, node);
	uint32_t best = 0, cur = e->pop(ctx);
	while (cur) {
		if (!best || R16(cur + 6) < R16(best + 6) || (R16(cur + 6) == R16(best + 6) && R16(best + 8) + 5 < R16(cur + 8))) best = cur;
		if (R16(cur + 6) == 0 || !e->explore(cur, info, ctx, target)) break;
		cur = e->pop(ctx);
	}
	return best ? e->flush(info, best) : 0;
}
