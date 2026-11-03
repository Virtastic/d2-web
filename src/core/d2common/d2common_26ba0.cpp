// 1.13c D2Common+0x26BA0 (ITEMS_GetRunesTxtRecordFromItem): runeword record of a socketed item. Guest structs are 32-bit images;
// callees are seams: 0x719A0 (class row, stack idx), 0x26080 (socket count, stack item), 0x24430 (item-type check, stack item + signed short).
// count/base are the runes.txt globals at RVA 0x9FBD4 / 0x9FBD8. Returns the guest record address or 0.
#include <cstdint>
#include <cstring>
static inline uint32_t rd(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
extern "C" uint32_t d2common_26ba0_port(uint32_t item, int32_t count, uint32_t base,
    uint32_t (*f719A0)(int32_t), uint32_t (*f26080)(uint32_t), int32_t (*f24430)(uint32_t, int32_t))
{
	if (!item) return 0;
	if (rd(item) == 4) {
		uint32_t d = rd(item + 0x14);
		if (d) { int32_t q = (int32_t)rd(d); if (q >= 4 && q <= 9) return 0; }
	}
	uint32_t row = f719A0((int32_t)rd(item + 4));
	if ((rd(row + 0x128) >> 16 & 0xFF) != 0) return 0;   // byte at +0x12A (dword +0x128 bits 16..23)
	uint32_t inv = rd(item + 0x60);
	if (!inv || rd(inv) != 0x1020304) return 0;
	uint32_t u = rd(inv + 0xC);
	if (!u) return 0;
	int32_t ids[6]; int32_t n = 0;
	for (;;) {
		if (rd(u) != 4) return 0;
		ids[n++] = (int32_t)rd(u + 4);
		uint32_t d = rd(u + 0x14);
		if (!d) break;
		uint32_t a = d + 0x5C;
		u = rd(a + 8);
		if (!u) break;
		if (n >= 6) break;   // retail overruns its 6-slot frame; the corpus never reaches it
	}
	uint32_t sock = f26080(item) & 0xFF;
	if ((int32_t)sock != n) return 0;
	for (int32_t i = 0; i < count; ++i) {
		uint32_t r = base + (uint32_t)i * 0x120;
		if (!(rd(r + 0x80) & 0xFF)) continue;
		int32_t k = 0;
		for (; k < 6; ++k) {
			int32_t v = (int32_t)rd(r + 0x98 + 4 * k);
			if (v < 1) break;
			if (n < k || ids[k] != v) { k = -1; break; }
		}
		if (k < 0 || k < (int32_t)sock) continue;
		bool bad = false;
		for (int j = 0; j < 3; ++j) {
			int16_t s; memcpy(&s, (const void*)(uintptr_t)(r + 0x92 + 2 * j), 2);
			if (!s) break;
			if (f24430(item, s)) { bad = true; break; }
		}
		if (bad) continue;
		for (int j = 0; j < 6; ++j) {
			int16_t s; memcpy(&s, (const void*)(uintptr_t)(r + 0x86 + 2 * j), 2);
			if (!s) break;
			if (f24430(item, s)) return r;
		}
	}
	return 0;
}
