// 1.13c D2Common+0x84220 (stdcall p1..p7, ret 0x1C): stamp rect p5 {x,y,w,h} of room p6 into the layer grids of p4, then call 0x83630.
// p4[0] = level A (+0x34/+0x38 origin, +0x54 = S), p4[+0x24] = grid {base, rowtable}, p4[1+l] / p4[5+l] = per-layer grids; room p6: +0x50 = Q
// (+0x14 layer count, +0x18 gate), +0xF4/+0xF8 flag array and row table, +0x108/+0x10C tile array and row table, layer l at +0x58+0x14*l
// ([lay-4] value array, [lay] its row table, [lay+0x4C] bit array, [lay+0x50] its row table). x0 = rect.x + p7. Pass 1 counts tiles with
// bit 0x8 in byte 3 of the tile word and calls 0x67FD0(ecx=p1, edi=count, esi=S). Pass 2 per cell: a flag word with bit 2 stores the flags|0x80
// into p4[+0x24] at cell (p3+j, p2+i) (0x36C20 = *ecx = edx); per layer a set bit 0 stores the bit word into p4[5+l], a non-zero value
// word into p4[1+l]; a tile word with bit 0x8000000 calls 0x68CD0(A, A.x+i+p2, 0, A.y+j+p3, 0x68B90(tile, A, 0xD)). Finally
// 0x83630(eax=p5, ecx=p3, edx=p2, p4, p1, p6). Callees are seams (d2common_84220.h).
#include <cstdint>
#include <cstring>
#include "d2common_84220.h"

static inline int32_t rd32(uint32_t b, uint32_t o) { int32_t v; std::memcpy(&v, (const void*)(uintptr_t)(b + o), 4); return v; }
static inline uint32_t ru32(uint32_t b, uint32_t o) { return (uint32_t)rd32(b, o); }

extern "C" void d2c_84220_run(uint32_t p1, int p2, int p3, uint32_t p4, uint32_t p5, uint32_t p6, int p7, const D2c84220Env* e)
{
	const uint32_t A = ru32(p4, 0);
	const uint32_t S = ru32(A, 0x54);
	const int layers = rd32(ru32(p6, 0x50), 0x14);
	const int x0 = rd32(p5, 0) + p7;
	const int w = rd32(p5, 8), h = rd32(p5, 12);
	uint32_t count = 0;
	for (int j = 0; j < h; ++j) {
		const uint32_t ro = (uint32_t)(rd32(p5, 4) + j) * 4;
		for (int i = 0; i < w; ++i)
			if (*(const uint8_t*)(uintptr_t)(ru32(p6, 0x108) + (uint32_t)(rd32(ru32(p6, 0x10c), ro) + i + x0) * 4 + 3) & 8) ++count;
	}
	e->f67fd0((uint32_t)p1, count, S);
	for (int j = 0; j < h; ++j) {
		const uint32_t ro = (uint32_t)(rd32(p5, 4) + j) * 4;
		const uint32_t yo = (uint32_t)(p3 + j) * 4;
		for (int i = 0; i < w; ++i) {
			uint32_t f = rd32(ru32(p6, 0x50), 0x18) ? ru32(ru32(p6, 0xf4), (uint32_t)(rd32(ru32(p6, 0xf8), ro) + i + x0) * 4) : 0;
			auto cellPtr = [&](uint32_t g) { return ru32(g, 0) + (uint32_t)(rd32(ru32(g, 4), yo) + i + p2) * 4; };
			if (f & 2) e->store(cellPtr(ru32(p4, 0x24)), f | 0x80);
			uint32_t lay = p6 + 0x58, cur = p4 + 4;
			for (int l = 0; l < layers; ++l, lay += 0x14, cur += 4) {
				uint32_t t = ru32(ru32(lay, 0x4c), (uint32_t)(rd32(ru32(lay, 0x50), ro) + i + x0) * 4);
				if ((t & 1) && ru32(cur, 0x10)) e->store(cellPtr(ru32(cur, 0x10)), t);
				t = ru32(ru32(lay, (uint32_t)-4), (uint32_t)(rd32(ru32(lay, 0), ro) + i + x0) * 4);
				if (t && ru32(cur, 0)) e->store(cellPtr(ru32(cur, 0)), t);
			}
			const uint32_t tile = ru32(ru32(p6, 0x108), (uint32_t)(rd32(ru32(p6, 0x10c), ro) + i + x0) * 4);
			if (tile & 0x8000000) {
				const int y = rd32(A, 0x38) + j + p3, x = rd32(A, 0x34) + i + p2;
				const uint32_t v = e->f68b90(tile, A, 0xd);
				e->f68cd0(A, x, 0, y, v);
			}
		}
	}
	e->f83630(p5, (uint32_t)p3, (uint32_t)p2, p4, p1, p6);
}
