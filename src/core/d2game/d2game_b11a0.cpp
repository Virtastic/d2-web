#include <unistd.h>
#include "../compat/test_seam.h"
#include "d2game_b11a0.h"
#include "d2game_retail_rw.h"
uint32_t D2R_B11A0_Room(uint32_t) { __builtin_trap(); }
void     D2R_B11A0_Units(uint32_t, void**, uint32_t*) { __builtin_trap(); }
uint32_t D2R_B11A0_E03A0(uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_B11A0_Hit(uint32_t, uint32_t, uint32_t, uint32_t) { __builtin_trap(); }

// 1.13c D2Game+0xB11A0: room sweep. D2Common #10331(arg2) is the room; #10383 yields its unit array + count. Each unit's path node at
// +0x174 is resolved: live flag bit 0 at +0x3D4 uses it as is, otherwise 0xE03A0(+0x1A8 base, +0x170 pos, +0x16C index) re-resolves it
// and a zero answer clears the field (a null unit in the array is retail's fatal 0xFB). Every non-null node is offered to the callback;
// the first non-zero answer is returned.
int32_t D2Retail113_B11A0(uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
	const uint32_t room = D2_CALL(D2R_B11A0_Room, arg2);
	if (room == 0) return 0;
	void* array = nullptr;
	uint32_t count = 0;
	D2_CALL(D2R_B11A0_Units, room, &array, &count);
	for (uint32_t i = 0; i < count; ++i) {
		const uint32_t record = rw32((uint32_t)(uintptr_t)array, i * 4);
		const uint32_t innerCount = rw32(record, 0x78);
		const uint32_t innerArray = rw32(record, 0x48);
		for (uint32_t j = 0; j < innerCount; ++j) {
			const uint32_t unit = rw32(innerArray, j * 4);
			if (unit == 0) _exit(-1);   // Ordinal_10265(0xFB) -> Ordinal_10024 -> _exit(-1)
			uint32_t node;
			if ((rw32(unit, 0x3D4) & 1) == 0) {
				if (rw32(unit, 0x174) == 0) continue;
				node = D2_CALL(D2R_B11A0_E03A0, rw32(unit, 0x1A8), rw32(unit, 0x170), rw32(unit, 0x16C));
				if (node == 0) ww32(unit, 0x174, 0);
			} else {
				node = rw32(unit, 0x174);
			}
			if (node != 0) {
				const uint32_t answer = D2_CALL(D2R_B11A0_Hit, arg1, arg2, node, arg3);
				if (answer != 0) return (int32_t)answer;
			}
		}
	}
	return 0;
}
