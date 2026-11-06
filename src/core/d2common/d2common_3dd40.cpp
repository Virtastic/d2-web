// 1.13c D2Common+0x3DD40, written from the 1.13c disassembly. Level +0x14 = outdoor block od, +0x1C / +0x20 = origin.
// od byte 0 bit 0x10, +0x5C / +0x60 = width / height, +0x2C / +0x30 = flag cells / row offsets, +0x260 = path count n,
// +0x80 + i*0x14 = first-set vertex i, +0x170 + i*0x14 = start vertex i, +0x1E8 + i*0x14 = second-set vertex i (+8 = kind).
// With the bit set the finder (0x7EA40) is asked for a cell on row width/2 - 1. A found cell (x != -1) places every
// second-set vertex at that cell's world point, shifted right by 8 with kind 0 when it lies left of the first-set vertex,
// else kind 2. Otherwise vertex 0 goes to the first free cell (flags & 0x1B81 clear) on the radius-0..7 cross around the
// centre (the grid middle for one path, else the mean first-set offset over 8n) and every vertex copies it, kind 4; a
// search that finds nothing keeps the last probed cell. Then every start vertex is converted with 0x3D4A0.
#include <cstdint>
#include "d2common_3dd40.h"

namespace {
inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
inline int32_t ri(uint32_t a) { return int32_t(rd(a)); }
inline void wr(uint32_t a, uint32_t v) { *reinterpret_cast<uint32_t*>(uintptr_t(a)) = v; }
inline void wb(uint32_t a, uint8_t v) { *reinterpret_cast<uint8_t*>(uintptr_t(a)) = v; }
const int32_t kTx[4] = { -1, 0, 0, 1 };   // 0x94050
const int32_t kTy[4] = { 0, 1, -1, 0 };   // 0x94040
}

extern "C" void D2Common_3DD40_With(uint32_t level, D2Common3DD40Find find, D2Common3DD40Coords coords)   // 1.13c D2Common+0x3DD40
{
	const uint32_t od = rd(level + 0x14);
	int32_t fx = -1, fy = -1;
	bool done = false;
	if (*reinterpret_cast<const uint8_t*>(uintptr_t(od)) & 0x10) {
		find(level, ri(od + 0x5c) / 2 - 1, &fx, &fy);
		if (fx != -1) {
			const int32_t x = ri(level + 0x1c) + fx * 8 + 3, y = ri(level + 0x20) + fy * 8 + 3;
			for (int32_t i = 0; i < ri(od + 0x260); ++i) {
				const uint32_t v = od + 0x1e8 + uint32_t(i) * 0x14;
				wr(v + 4, uint32_t(y));
				if (ri(v - 0x168) > x) { wr(v, uint32_t(x + 8)); wb(v + 8, 0); }
				else { wr(v, uint32_t(x)); wb(v + 8, 2); }
			}
			done = true;
		}
	}
	if (!done) {
		int32_t px = fx, py = fy;
		for (int32_t i = 0; i < ri(od + 0x260); ++i) {
			if (i) {
				const uint32_t v = od + uint32_t(i) * 0x14;
				wr(v + 0x1e8, rd(od + 0x1e8)); wr(v + 0x1ec, rd(od + 0x1ec)); wb(v + 0x1f0, 4);
				continue;
			}
			const int32_t n = ri(od + 0x260);
			int32_t cx, cy;
			if (n == 1) { cx = ri(od + 0x5c) / 2; cy = ri(od + 0x60) / 2; }
			else {
				int32_t sx = 0, sy = 0;
				for (int32_t j = 0; j < n; ++j) {
					sx += ri(od + 0x80 + uint32_t(j) * 0x14) - ri(level + 0x1c);
					sy += ri(od + 0x84 + uint32_t(j) * 0x14) - ri(level + 0x20);
				}
				const int32_t d = int32_t(uint32_t(n) << 3);
				cx = sx / d; cy = sy / d;
			}
			bool hit = false;
			for (int32_t r = 0; r < 8 && !hit; ++r)
				for (int32_t k = 0; k < 4; ++k) {
					px = kTx[k] * r + cx; py = kTy[k] * r + cy;
					if (px >= 0 && px < ri(od + 0x5c) && py >= 0 && py < ri(od + 0x60)) {
						const uint32_t g = rd(level + 0x14);
						const uint32_t cell = rd(g + 0x2c) + uint32_t(ri(rd(g + 0x30) + uint32_t(py) * 4) + px) * 4;
						if (!(rd(cell) & 0x1b81)) { hit = true; break; }
					}
				}
			wr(od + 0x1e8, uint32_t(ri(level + 0x1c) + px * 8 + 3));
			wr(od + 0x1ec, uint32_t(ri(level + 0x20) + py * 8 + 3));
			wb(od + 0x1f0, 4);
		}
	}
	for (int32_t i = 0; i < ri(od + 0x260); ++i)
		coords(level, od + 0x170 + uint32_t(i) * 0x14, od + 0x1e8 + uint32_t(i) * 0x14);
}
