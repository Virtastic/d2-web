// D2Common 0x4A5C0: add an outdoor (maze) room rectangle to a level. Callees and the two table globals arrive through Env.
#pragma once
#include <cstdint>
struct D2Common4a5c0Env {
	uint32_t (*allocRoom)(uint32_t level, uint32_t type);                 // 0x44BD0: EBX level, stack type (1); returns the room
	uint32_t (*pickSubThemes)(uint32_t room, uint32_t subType, uint32_t subTheme);   // 0x83740: ESI room, stack both
	uint32_t dataTables;   // sgptDataTables (+0xC5C = levels row count)
	uint32_t levelDefs;    // the LvlPrest-side level def rows (stride 0x9C; +0x38 sub type, +0x3C sub theme)
};
extern "C" uint32_t D2Common_4A5C0_With(uint32_t flags, uint32_t level, uint32_t x, uint32_t y, uint32_t w, uint32_t h,
	uint32_t outFlags, uint32_t outFlagsEx, uint32_t dt1Mask, const D2Common4a5c0Env* e);
