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

// 1.13c D2Common+0x4E750 (#10864, community name D2GetMaxGoldBank): a thin wrapper over the collision
// ray walk 0x4AD20 (still UNMAPPED, hence the seam). The first four values are the start point
// (nFromX, nFromY) and the end point (nToX, nToY), the last the collision mask; the first argument is
// the collision record 0x4AD20 walks from. The answer is true when the walk finds nothing (result 0).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_4e750(uint32_t pCollision, int32_t nFromX, int32_t nFromY,
	int32_t nToX, int32_t nToY, uint16_t nMask,
	int32_t (*pfnTest4AD20)(uint32_t, const int32_t*, const int32_t*, uint16_t))
{
	const int32_t from[2] = { nFromX, nFromY };
	const int32_t to[2] = { nToX, nToY };

	if (!pfnTest4AD20)
		return 0;

	return pfnTest4AD20(pCollision, from, to, nMask) == 0 ? 1u : 0u;
}
