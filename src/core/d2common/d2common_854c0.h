// Callee and table seams of D2Common 0x854C0 (see d2common_854c0.cpp).
#pragma once
#include <cstdint>
struct D2c854C0Env {
	uint32_t dataTables;                    // sgptDataTables (pointer global 0x99E1C); +0xC5C = level-def count
	uint32_t levelDefBase;                  // pointer global 0xA0B38; the level-def record of type 0x4C is at +0x2E50
	const int32_t* layerTab;                // table at 0x90168: three dwords per roll (link-slot values)
	const int32_t* typeOff;                 // table at 0x90080 read at type*4: the three entries for types 0x4C..0x4E
	uint32_t (*roll)(uint32_t seed, int32_t range);                                       // 0x10B0, EAX = range, ECX = &seed
	uint32_t (*place)(uint32_t lvl, int32_t x, int32_t y, int32_t tile, int32_t flag, int32_t last);   // 0x42A70, ECX = lvl, EDX = x, 4 stack words; EAX at its return
	void (*fatal)(int32_t code);            // 0x921C + 0x9216 + _exit: code 0x27 or 0x47 (retail never returns)
};
// Returns the word retail leaves in EAX (see d2common_854c0.cpp).
extern "C" uint32_t d2c_854c0_run(uint32_t level, const D2c854C0Env* env);
