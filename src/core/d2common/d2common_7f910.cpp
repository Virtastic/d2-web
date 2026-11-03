// 1.13c D2Common+0x7F910: stdcall (unit, cb, arg), ret 0xC. Returns the first nearby unit for which cb(ECX node, EDX arg) is nonzero, else 0.
// size = 0x80F80(unit) must be 1..3; room = 0x7FE10(unit) nonzero; x/y come from the unit's +0x2C data (dwords +0xC/+0x10 for unit type 2 and 4..5,
// else the words +2/+6, 0 with no data). 0x8BA20(room, &arr, &n) lists n entries; for each one that 0x7F490 (ECX entry, EAX y, EDX x, EDI 2) accepts,
// walk its list (+0x74, next at +0xE8): types 0, 1 (data +0x10 not 0 or 0x11/0xC) and 3 count, the node's own size (0x80F80, clamped to 3) and the caller's
// pick one of nine closeness tests (idx = nodeSize - 1 + 3 * (size - 1)); a node other than unit that passes the test is offered to cb.
#include <cstdint>
typedef int32_t (*D2C7F910Size)(uint32_t unit);
typedef uint32_t (*D2C7F910Room)(uint32_t unit);
typedef void (*D2C7F910List)(uint32_t room, uint32_t* arr, int32_t* n);
typedef int32_t (*D2C7F910Hit)(uint32_t obj, int32_t y, int32_t x);
typedef int32_t (*D2C7F910Cb)(uint32_t node, uint32_t arg);
static inline uint32_t rd(uint32_t a, uint32_t o) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a) + o); }
static inline uint32_t rw(uint32_t a, uint32_t o) { return *reinterpret_cast<const uint16_t*>(uintptr_t(a) + o); }
static inline void pos(uint32_t u, int32_t& x, int32_t& y)
{
	const uint32_t t = rd(u, 0), d = rd(u, 0x2C);
	const bool big = t == 2 || (t > 3 && t < 6);
	x = big ? int32_t(rd(d, 0xC)) : (d ? int32_t(rw(d, 2)) : 0);
	y = big ? int32_t(rd(d, 0x10)) : (d ? int32_t(rw(d, 6)) : 0);
}
static inline int32_t ab(int32_t v) { return v < 0 ? -v : v; }
extern "C" uint32_t d2c_7f910_port(uint32_t u, D2C7F910Cb cb, uint32_t arg, D2C7F910Size f80f80, D2C7F910Room f7fe10, D2C7F910List f8ba20, D2C7F910Hit f7f490)
{
	const int32_t size = f80f80(u);
	if (size < 1 || size > 3) return 0;
	const int32_t base = size * 3 - 3;
	const uint32_t room = f7fe10(u);
	if (!room) return 0;
	int32_t x, y;
	pos(u, x, y);
	uint32_t arr = 0; int32_t n = 0;
	f8ba20(room, &arr, &n);
	for (int32_t i = 0; i < n; i++) {
		const uint32_t e = rd(arr, uint32_t(i) * 4);
		if (!f7f490(e, y, x)) continue;
		for (uint32_t p = rd(e, 0x74); p; p = rd(p, 0xE8)) {
			const uint32_t t = rd(p, 0);
			if (t == 0) { const uint32_t v = rd(p, 0x10); if (v == 0 || v == 0x11) continue; }
			else if (t == 1) { const uint32_t v = rd(p, 0x10); if (v == 0 || v == 0xC) continue; }
			else if (t != 3) continue;
			int32_t ns = f80f80(p);
			if (ns >= 3) ns = 3; else if (ns <= 0) continue;
			int32_t nx, ny;
			pos(p, nx, ny);
			bool hit = false;
			const int32_t dx = ab(x - nx), dy = ab(y - ny);
			switch (ns - 1 + base) {
			case 0: hit = x == nx && y == ny; break;
			case 1: case 3: hit = dx + dy <= 1; break;
			case 4: hit = dx + dy <= 2; break;
			case 2: case 6: hit = nx - 1 <= x && x <= nx + 1 && ny - 1 <= y && y <= ny + 1; break;
			case 5: case 7: hit = (dx < 3 && dy < 2) || (dy < 3 && dx <= 1); break;
			case 8: hit = nx - 2 <= x && x <= nx + 2 && ny - 2 <= y && y <= ny + 2; break;
			default: break;
			}
			if (hit && p != u && cb(p, arg)) return p;
		}
	}
	return 0;
}
