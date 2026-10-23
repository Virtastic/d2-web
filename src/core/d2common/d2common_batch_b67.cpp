// src/core/d2common/d2common_batch_b67.cpp -- the one 1.13c D2Common.dll row of batch b67 whose body
// is not already the vendored D2MOO function, transcribed from the cached Ghidra decompile
// (build-wasm/ghidra-d2common/decomp/0x3B160.c) and checked against the retail disassembly.
// Image base 0x6FD50000. Retail records are opaque and are read by their retail byte offsets
// (rd32); retail addresses travel as 32-bit integers.
//
// The other three rows of the batch are the vendored bodies, marked in place:
//   0x34550 -> src/core/d2moo/src/Path/Path.cpp D2Common_10237
//   0x2F630 -> src/core/d2moo/src/Units/Units.cpp UNITS_GetSwitchLeftSkillData
//   0x3C460 -> src/core/d2moo/src/D2Dungeon.cpp DUNGEON_AddClientToRoom
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace
{
	inline uint32_t rd32(uint32_t a, uint32_t o)
	{
		uint32_t v;
		std::memcpy(&v, reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)) + o, 4);
		return v;
	}
}

// 0x19860(key, owner) is the verified 1.13c row of the same DLL; its probe entry takes the pair as
// a packed record { key, owner, 0, 0, 0 }.
extern "C" void d2_fn_d2common_19860(uint32_t* pArgs);

// 1.13c D2Common+0x3b160 (#10880)
// Two stdcall arguments: a record and a key (disassembly: [esp+4] -> ecx, [esp+8] -> eax). A null
// record is the 1.13c assert at line 0x5C4 (fatal); it returns here. The owner pointer is the
// record's dword at +0x10. The body forwards (key, owner) to 0x19860 unchanged.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_3b160(uint32_t pRecord, uint32_t nKey)
{
	if (pRecord == 0) {
		return;
	}

	uint32_t args[5] = { nKey, rd32(pRecord, 0x10), 0, 0, 0 };
	d2_fn_d2common_19860(args);
}
