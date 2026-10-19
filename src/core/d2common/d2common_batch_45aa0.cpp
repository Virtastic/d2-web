#include <cstdint>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "DataTbls/TokenTbls.h"

// Prior art used: D2MOO's DATATBLS_LoadObjType_ModeTxt in DataTbls/TokenTbls.cpp is the 1.10f
// twin of 0x19190. The 1.13c decompile confirms the same objtype/objmode records, 0x34-byte
// stride, contiguous destination allocation and split type/mode pointers. OpenD2/D2MOO layouts
// are the table-layout reference per .
namespace
{
	inline uint8_t* P(uint32_t address)
	{
		return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(address));
	}

	inline int32_t I(uint32_t address)
	{
		return *reinterpret_cast<const int32_t*>(P(address));
	}

	inline uint32_t U(uint32_t address)
	{
		return *reinterpret_cast<const uint32_t*>(P(address));
	}
}

// The x86 entry also receives unit in EAX and the property-function record in EDI. Expose those
// register inputs explicitly at the wasm boundary. fatal() models retail's error-message + _exit
// paths and is expected not to return.
struct D2Common45AA0Env {
    int32_t propertyFunctionCount;
    void (**propertyFunctions)();
    void (*applyProperty)(int32_t, int32_t, const int32_t*);
    void (*fatal)(uint32_t);
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_45aa0(
    int32_t nArg1, int32_t nArg2, const int32_t* unit, const int32_t* propertyFunction,
    const D2Common45AA0Env* env) { // 1.13c D2Common+0x45AA0
    if (!unit) {
        if (env && env->fatal) env->fatal(0x1505);
        return;
    }
    if (unit[0] != 4) {
        if (env && env->fatal) env->fatal(0x1506);
        return;
    }
    if (unit[5] == 0) {
        if (env && env->fatal) env->fatal(0x1508);
        return;
    }

    const auto* item = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(static_cast<uint32_t>(unit[5])));
    if (*reinterpret_cast<const int16_t*>(item + 0x30) != 0) {
        if (env && env->applyProperty) env->applyProperty(nArg1, nArg2, unit);
        return;
    }
    if (!propertyFunction) {
        if (env && env->fatal) env->fatal(0x066d);
        return;
    }

    const int32_t index = propertyFunction[0];
    if (env && env->propertyFunctions && index >= 0 && index < env->propertyFunctionCount &&
        env->propertyFunctions[index]) {
        env->propertyFunctions[index]();
    }
}

// 1.13c D2Common+0x819C0: read the tile entry at a room coordinate. The input coordinates are
// already in subtiles; the row-offset table is relative to room+0x38 and the selected cell is
// relative to room+0x34. A flat grid (flag bit 0) uses its sole cell at +0x30. A missing grid
// follows retail's fatal 0x29C path; the staged function returns 0 after that modeled fatal.
uint32_t d2_fn_d2common_819c0(int32_t nSubY, uint32_t pRoom, int32_t nSubX) { // 1.13c D2Common+0x819C0
	const uint32_t pGrid = U(pRoom + 0x64);
	if (pGrid == 0)
		return 0;

	if ((U(pGrid) & 1u) != 0)
		return U(pGrid + 0x30);

	const uint32_t pRowOffsets = U(pGrid + 0x20);
	const uint32_t pCells = U(pGrid + 0x1C);
	const uint32_t nRow = static_cast<uint32_t>(nSubY) - static_cast<uint32_t>(I(pRoom + 0x38));
	const int32_t nRowStart = I(pRowOffsets + nRow * 4u);
	const uint32_t nColumn = static_cast<uint32_t>(nRowStart)
		- static_cast<uint32_t>(I(pRoom + 0x34))
		+ static_cast<uint32_t>(nSubX);
	const uint32_t pTile = U(pCells + nColumn * 4u);
	return pTile != 0 ? U(pTile + 0x28) : 0xFFFFFFFFu;
}

// 1.13c D2Common+0x19190: load objtype and objmode rows into one contiguous 0x34-byte-record
// allocation and publish its split pointers. This is the same loader recovered in D2MOO's
// DATATBLS_LoadObjType_ModeTxt; the archive handle arrives in EAX in the retail entry point.
void d2_fn_d2common_19190(HD2ARCHIVE hArchive) { // 1.13c D2Common+0x19190
	DATATBLS_LoadObjType_ModeTxt(hArchive);
}
