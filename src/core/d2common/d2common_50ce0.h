// D2Common 0x50CE0 (#10639): a player's attack frames per division (frame word masked to 0xFFFFFF00 / weapon speed or 19).
#pragma once
#include <cstdint>
struct D2Common50CE0Env {
	uint32_t (*weaponItem)(uint32_t pUnit);                      // 0x31B90 stdcall(unit, 0): the wielded item (0 = none)
	int32_t (*weaponSpeed)(uint32_t pUnit, uint32_t pItem);      // 0x267F0 stdcall(unit, item)
};
extern "C" int32_t D2Common_50CE0_FrameStep(uint32_t pUnit, const D2Common50CE0Env* env);
