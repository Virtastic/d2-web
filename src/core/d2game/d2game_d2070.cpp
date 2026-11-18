#include "../compat/test_seam.h"
#include "d2game_d2070.h"
#include "d2game_retail_rw.h"
uint32_t D2R_D2070_Quality(uint32_t) { __builtin_trap(); }
uint32_t D2R_D2070_FileIndex(uint32_t) { __builtin_trap(); }
uint32_t D2R_D2070_UniqueRow(uint32_t) { __builtin_trap(); }
uint32_t D2R_D2070_Mask8() { __builtin_trap(); }
uint32_t D2R_D2070_IsItem(uint32_t) { __builtin_trap(); }
uint32_t D2R_D2070_Next(uint32_t) { __builtin_trap(); }
uint32_t D2R_D2070_InvPage(uint32_t) { __builtin_trap(); }
uint32_t D2R_D2070_TxtRow(uint32_t) { __builtin_trap(); }

static inline bool pair(uint32_t a, uint32_t b, uint32_t x, uint32_t y) { return (a == x && b == y) || (a == y && b == x); }

// 1.13c D2Game+0xD2070 -- EAX = list head, EBX = items.txt row of `item`, stack = item (ret 4). Same carry-1 unique, or a same-group quest
// item (row+0x12A) whose code (row+0x80) is equal or a sibling pair; the walk stops at `item` itself. Codes are little-endian four-char tags.
uint32_t D2Retail113_D2070(uint32_t head, uint32_t row, uint32_t item)
{
	int32_t uniq = -1;
	if (item != 0 && rw32(item, 0) == 4 && D2_CALL(D2R_D2070_Quality, item) == 7) {
		int32_t idx =(int32_t)D2_CALL(D2R_D2070_FileIndex, item);
		if (idx >= 0 && (idx = (int32_t)D2_CALL(D2R_D2070_FileIndex, item)) >= 0) {
			const uint32_t rec = D2_CALL(D2R_D2070_UniqueRow, (uint32_t)idx);
			if (rec != 0 && (D2_CALL(D2R_D2070_Mask8) & (rw32(rec, 0x2c) & 0xff)) != 0) uniq = (int16_t)rw16(rec, 0);
		}
	}
	for (uint32_t node = head; node != 0;) {
		const uint32_t it = D2_CALL(D2R_D2070_IsItem, node);
		node = D2_CALL(D2R_D2070_Next, node);
		if (it == item) break;
		if (it == 0 || rw32(it, 0) != 4) continue;
		if (uniq >= 0 && (D2_CALL(D2R_D2070_InvPage, it) & 0xff) != 1 && D2_CALL(D2R_D2070_Quality, it) == 7 &&
		    (int32_t)D2_CALL(D2R_D2070_FileIndex, it) == uniq)
			return 1;
		const uint32_t r2 = D2_CALL(D2R_D2070_TxtRow, rw32(it, 4));
		const uint8_t grp = (uint8_t)rw32(r2, 0x12a);
		if (grp != 0 && (D2_CALL(D2R_D2070_InvPage, it) & 0xff) != 1 && grp == (uint8_t)rw32(row, 0x12a)) {
			const uint32_t a = rw32(row, 0x80), b = rw32(r2, 0x80);
			if (a == b || pair(a, b, 0x2034336a, 0x20343367) || pair(a, b, 0x20736b62, 0x20646b62) || pair(a, b, 0x20333364, 0x20333367) ||
			    pair(a, b, 0x20747368, 0x2066736d) || pair(a, b, 0x20747368, 0x20706976) || pair(a, b, 0x20326671, 0x20316671) ||
			    pair(a, b, 0x20326671, 0x20726871) || pair(a, b, 0x20326671, 0x20796571) || pair(a, b, 0x20326671, 0x20726271))
				return 1;
		}
	}
	return 0;
}
