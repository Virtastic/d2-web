// D2Common 0x47730 (#10914): prepare an item unit and seed it when its item data carries flag 0x400000.
#pragma once
#include <cstdint>
struct D2Common47730Env {
	void (*prep)(uint32_t pUnit, uint32_t nArg); // 0x45AA0: EAX = unit, ECX = 0, EDX = 0x40, stack (7, 0, nArg, 0, 0, 0, 0), ret 0x1C
	void (*seed)(uint32_t pUnit);                // 0x46A40 stdcall(unit)
};
extern "C" void D2Common_47730_SeedItem(uint32_t pUnit, uint32_t nArg, const D2Common47730Env* env);
