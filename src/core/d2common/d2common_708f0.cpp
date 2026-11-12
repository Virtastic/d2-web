// 1.13c D2Common+0x708F0 on the retail 32-bit layout, the search walker 0x70BB0 calls (with 0x70840 and 0x707E0 under it).
// The vendored PATH_IDAStar_VisitNodes keeps the native layout; this copy works on 0x70BB0's contiguous search context:
//   +0 current node (u32), +4 900 nodes of 0x1C, +0x6274 node count, +0x6278 x0,y0,x1,y1 (u16), +0x6284 stride,
//   +0x6288 x offset, +0x628C y offset, +0x6290 cell costs (u32), +0x36FD0 random-direction flag, +0x36FD4 seed pointer.
// Node: +0 f (u16), +2 h (u16), +4 g (u16), +6 tries (s16), +8 x,y (u16), +0xC neighbour-sequence pointer (u32),
//   +0x10 next neighbour index, +0x14 parent (u32), +0x18 child (u32).
// Path info (retail words): +4 target point, +8 collision map, +0x14 byte stop distance, +0x28 pattern, +0x2C mask.
#include <cstdint>
#include <cstring>

namespace {
const int32_t kStep[8][2] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};  // DAT_6fddf508
const int32_t kSeq[8][8] = {                                                                          // DAT_6fddb860
	{0, 1, 6, 3, 4, 5, 2, 7}, {0, 1, 6, 3, 1, 3, 6, 1}, {0, 1, 1, 1, 1, 1, 2, 7}, {1, 1, 1, 1, 1, 3, 6, 1},
	{6, 4, 3, 6, 1, 3, 2, 7}, {7, 7, 7, 7, 7, 5, 2, 7}, {0, 7, 7, 7, 7, 5, 2, 7}, {0, 7, 2, 5, 7, 5, 2, 7}};
const int32_t kRand[32] = {-2, -1, 0, 1, 2, -2, -1, 0, 1, 2, -2, -1, 0, 1, 2, -2,  // DAT_6fddb960
                           -1, 0,  1, 2, -2, -1, 0, 1, 2, -2, -1, 0, 1, 2, -1, 1};
const int32_t kDir[25] = {5, 4, 4, 4, 3, 6, 5, 4, 3, 2, 6, 6, 6, 2, 2, 6, 7, 0, 1, 2, 7, 0, 0, 0, 1};  // DAT_6fddc320 +0 (12-byte rows)

inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
inline uint32_t R32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, P(a + o), 4); return v; }
inline uint16_t R16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, P(a + o), 2); return v; }
inline void W32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy(P(a + o), &v, 4); }
inline void W16(uint32_t a, uint32_t o, uint16_t v) { std::memcpy(P(a + o), &v, 2); }

uint32_t Roll(uint32_t ctx)  // the seed at ctx+0x36FD4: lo * 0x6AC690C5 + hi, returns the new low word
{
	const uint32_t s = R32(ctx, 0x36FD4);
	const uint64_t v = uint64_t(R32(s, 0)) * 0x6AC690C5u + R32(s, 4);
	W32(s, 0, uint32_t(v)); W32(s, 4, uint32_t(v >> 32));
	return uint32_t(v);
}

// 0x707E0: advance the node's neighbour index by its current sequence entry (plus a random offset when the flag is set).
void NextIndex(uint32_t ctx, uint32_t node)
{
	const int32_t seq = int32_t(R32(R32(node, 0xC), 0));
	int32_t add = 0;
	if (R32(ctx, 0x36FD0)) add = kRand[Roll(ctx) & 0x1F];
	W32(node, 0x10, uint32_t(add + seq + int32_t(R32(node, 0x10))) & 7);
}

// 0x70840: move on to the node's next neighbour; after the fifth try climb to the parent and do the same there; 0 at the root.
uint32_t Advance(uint32_t ctx, uint32_t node)
{
	if (int16_t(R16(node, 6)) < 4) { W32(node, 0xC, R32(node, 0xC) + 4); NextIndex(ctx, node); }
	W16(node, 6, uint16_t(R16(node, 6) + 1));
	while (int16_t(R16(node, 6)) == 5) {
		if (node == R32(ctx, 0)) return 0;
		node = R32(node, 0x14);
		W32(node, 0xC, R32(node, 0xC) + 4);
		NextIndex(ctx, node);
		W16(node, 6, uint16_t(R16(node, 6) + 1));
	}
	return node;
}
}  // namespace

int32_t d2_fn_d2common_3e190(uint32_t a, uint32_t b);  // 1.13c D2Common+0x3E190 (d2common_batch_b58b.cpp)
// 0x4CE90 collision probe (map, x, y, pattern, mask): nonzero = blocked. Unset = trap (the probe's vectors never reach it).
extern "C" __attribute__((weak)) uint32_t (*d2_env_d2common_708f0_coll)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) = nullptr;

// Address of the first neighbour sequence (0x70BB0's start node points at it).
extern "C" uint32_t d2c_708f0_seq0() { return uint32_t(reinterpret_cast<uintptr_t>(&kSeq[0][0])); }

extern "C" uint32_t d2c_708f0_retail(uint32_t ctx, int32_t cutoff, uint32_t pi)
{
	const uint32_t tgt = R32(pi, 4);
	const uint16_t tx = uint16_t(tgt), ty = uint16_t(tgt >> 16);
	uint32_t node = R32(ctx, 0);
	int32_t visits = 0;
	for (;;) {
		if (R16(node, 8) == tx && R16(node, 10) == ty) return node;
		if (++visits > 10000) return 0;
		const uint16_t cx = R16(node, 8), cy = R16(node, 10);
		if (cx < R16(ctx, 0x6278) || R16(ctx, 0x627C) < cx) return node;
		if (cy < R16(ctx, 0x627A) || R16(ctx, 0x627E) < cy) return node;
		const uint32_t dir = R32(node, 0x10);
		const uint16_t ny = uint16_t(int16_t(kStep[dir][1]) + cy);
		const uint16_t nx = uint16_t(int16_t(kStep[dir][0]) + cx);
		const int32_t idx = (int32_t(R32(ctx, 0x628C)) + ny) * int32_t(R32(ctx, 0x6284)) + int32_t(R32(ctx, 0x6288)) + nx;
		const uint32_t cell = ctx + 0x6290 + uint32_t(idx) * 4;
		const uint32_t packed = uint32_t(ny) << 16 | nx;
		bool open = R32(cell, 0) != 0;
		if (!open) {
			if (!d2_env_d2common_708f0_coll) __builtin_trap();
			open = d2_env_d2common_708f0_coll(R32(pi, 8), nx, ny, R32(pi, 0x28), R32(pi, 0x2C)) == 0;
			if (!open) W32(cell, 0, 1);
		}
		if (open) {
			const int16_t step = (cx == nx || cy == ny) ? 2 : 3;
			const int16_t g = int16_t(R16(node, 4) + step);
			const uint32_t cost = R32(cell, 0);
			if (!(cost != 0 && cost < uint32_t(int32_t(g)))) {
				W32(cell, 0, uint32_t(int32_t(g)));
				int32_t dx = int32_t(tx) - int32_t(nx); if (dx < 0) dx = -dx;
				int32_t dy = int32_t(ty) - int32_t(ny); if (dy < 0) dy = -dy;
				const int32_t lo = dy <= dx ? dy : dx, hi = dy <= dx ? dx : dy;
				const int16_t h = int16_t(lo + hi * 2);
				const int16_t f = int16_t(h + g);
				if (cutoff >= f) {
					uint32_t child = R32(node, 0x18);
					if (!child) {
						const uint32_t n = R32(ctx, 0x6274);
						if (n != 900) {
							W32(ctx, 0x6274, n + 1);
							child = ctx + 4 + n * 0x1C;
							std::memset(P(child), 0, 0x1C);
						}
						W32(node, 0x18, child);
						if (!child) return 0;
						W32(child, 0x14, node);
					}
					node = child;
					const uint32_t parent = R32(node, 0x14);
					W16(node, 4, uint16_t(g)); W16(node, 0, uint16_t(f)); W16(node, 2, uint16_t(h)); W16(node, 6, 0);
					const int32_t d = kDir[d2_fn_d2common_3e190(packed, tgt)];
					const int32_t* seq = kSeq[(d - int32_t(R32(parent, 0x10))) & 7];
					W32(node, 0xC, uint32_t(reinterpret_cast<uintptr_t>(seq)));
					int32_t add = 0;
					if (R32(ctx, 0x36FD0)) add = kRand[Roll(ctx) & 0x1F];
					W32(node, 0x10, uint32_t(add + int32_t(R32(parent, 0x10)) + seq[0]) & 7);
					W32(node, 8, packed);
					if (int16_t(R16(node, 2)) < int16_t(uint8_t(*P(pi + 0x14)))) return node;
					continue;
				}
			}
		}
		node = Advance(ctx, node);
		if (!node) return 0;
	}
}
