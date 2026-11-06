// 1.13c D2Common+0x84AD0 (fastcall, ecx = unused first argument, EDI = self): per-layer scatter dispatcher.
// self[0xC] = entry-table index (-1 = nothing to do), self[0xE] = layer bitmask; entries are 0x15C bytes apart, one per bit.
// The context handed to every layer is self[0], then +0x58, then +0x1B4, then +0x464 (a pointer chain, last word the context).
// For each set bit: 0x83CA0(ctx, entry), then 0x84500(self, entry) when entry+0x40 is zero, else 0x84890(self, entry).
// The entry base comes from 0x6C4D0 (EDX = index). Callees are seams so a probe can script them; the play path passes the
// real ones (d2common_batch_453d0.cpp).
#include <cstdint>
#include "d2common_84ad0.h"

namespace {
inline int32_t I32(const void* p, uint32_t off) { return *(const int32_t*)((const uint8_t*)p + off); }
inline void* P32(const void* p, uint32_t off) { return (void*)(uintptr_t)(uint32_t)I32(p, off); }
}

extern "C" void d2c_84ad0_run(int32_t* self, const D2c84AD0Env* e)
{
	if (self[0xC] == -1) return;
	uint8_t* ent = (uint8_t*)e->entries(self[0xC]);
	void* ctx = (void*)(uintptr_t)(uint32_t)I32(P32(P32(P32(self, 0), 0x58), 0x1B4), 0x464);
	for (uint32_t m = (uint32_t)self[0xE]; m; m >>= 1, ent += 0x15C)
		if (m & 1) {
			e->prep(ctx, ent);
			if (I32(ent, 0x40) == 0) e->flat(self, ent); else e->scatter(self, ent);
		}
}
