// 1.13c D2Common implementation batch (rows 0x321E0, 0x4E750, 0x4C940, 0x384B0).
// Bodies follow the cached 1.13c decompiles (build-wasm/ghidra-d2common/decomp/0x<RVA>.c); the
// register-only operands were recovered from the retail disassembly. Rows whose body is already the
// vendored 1.10f function are marked in place there instead of being duplicated:
//   0x321E0 = UNITS_GetHitClass (src/core/d2moo/src/Units/Units.cpp), no delta;
//   0x384B0 = STATLIST_GetStatListFromUnitStateOrFlag (src/core/d2moo/src/D2StatList.cpp), whose
//             policy is identical (its inner call re-checks the same extended-list guard).
// A callee that is still an UNMAPPED row is a function-pointer seam handed over last: a null seam
// contributes 0 and the exported body never faults.
#include <cstddef>
#include <cstdint>

#include <emscripten.h>

#include "D2Collision.h"

namespace
{
	// Grid record / layer prefix: three int32 fields then a pointer. The pointer still lands at byte
	// 0x20 under m64 because every field before it is int32 (the retail 32-bit build has it at 0x20).
	struct D2GridHeaderStrc
	{
		int32_t nField0;   // 0x00
		int32_t nField1;   // 0x04
		int32_t nField2;   // 0x08
		int32_t nPad[5];   // 0x0C
		void* pCells;      // 0x20
	};
	static_assert(offsetof(D2GridHeaderStrc, pCells) == 0x20, "grid header pointer at retail offset 0x20");
}

// 1.13c D2Common+0x4C940 (#10870): clears nFlags in the 16-bit grid cell at (nX, nY). The layer comes
// from the 0x1330 grid record (its +0x20 pointer, then the layer's own +0x20 cell array), the cell
// index is (nY - layer[+0x04]) * layer[+0x08] - layer[+0x00] + nX. The first argument is not read.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_4c940(uint32_t nUnused, int32_t nX, int32_t nY, uint16_t nFlags,
	void* (*pfnGrid1330)(void))
{
	(void)nUnused;
	if (!pfnGrid1330)
		return;

	auto* pRecord = static_cast<D2GridHeaderStrc*>(pfnGrid1330());
	auto* pLayer = pRecord ? static_cast<D2GridHeaderStrc*>(pRecord->pCells) : nullptr;
	if (pLayer == nullptr || pLayer->pCells == nullptr)
		return;

	const int32_t nIndex = (nY - pLayer->nField1) * pLayer->nField2 - pLayer->nField0 + nX;
	auto* pCell = reinterpret_cast<uint16_t*>(static_cast<uint8_t*>(pLayer->pCells) +
		static_cast<intptr_t>(nIndex) * 2);
	*pCell &= static_cast<uint16_t>(~nFlags);
}

// 1.13c D2Common+0x4e750 (#10864, community name D2GetMaxGoldBank): from the decompile, the start point
// (nFromX, nFromY) and end point (nToX, nToY) go to the collision ray walk 0x4AD20 (COLLISION_RayTrace,
// VECTORED) as (pCollision, &from, &to, nMask); the answer is true when the walk finds nothing.
// The former seam (a null function pointer that returned 0) is replaced by the direct call.
BOOL D2Common_4E750_GetMaxGoldBank(int32_t nFromX, int32_t nFromY, int32_t nToX, int32_t nToY,
	uint32_t pCollision, uint16_t nMask)
{
	D2CoordStrc from = { nFromX, nFromY };
	D2CoordStrc to = { nToX, nToY };

	return COLLISION_RayTrace(reinterpret_cast<D2ActiveRoomStrc*>(static_cast<uintptr_t>(pCollision)),
		&from, &to, nMask) ? FALSE : TRUE;
}
