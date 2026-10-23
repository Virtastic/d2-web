// 1.13c D2Common implementation batch: +0x2F570, +0x37470, +0x37660, +0x3BE90, +0x5F1C0.
// Bodies translated from the pinned 1.13c decompiles. Retail pointers are 32-bit image addresses in
// the offsets below; fields that have a vendored twin are read by name. Callees that are still
// UNMAPPED rows are env-seam function pointers (null until the environment installs them). Fatal
// paths (Fog message + _exit(-1)) are modelled as returns, as in the other d2common batch files.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <emscripten.h>

#include "D2Environment.h"
#include "Units/Player.h"
#include "Units/Units.h"

static_assert(offsetof(D2DrlgEnvironmentStrc, nCycleIndex) == 0x00, "environment cycle index");
static_assert(offsetof(D2DrlgEnvironmentStrc, nPeriodOfDay) == 0x04, "environment period of day");
static_assert(offsetof(D2DrlgEnvironmentStrc, nTicks) == 0x08, "environment ticks");
static_assert(offsetof(D2DrlgEnvironmentStrc, nTimeRate) == 0x28, "environment time rate");
static_assert(offsetof(D2DrlgEnvironmentStrc, nPrev) == 0x34, "environment previous minute");

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(void* b, int o) { return *reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(b) + o); }
}

// Env seams for callees that are still-UNMAPPED rows. Register use taken from the 1.13c call sites
// (0 and 2 take class id + buffer, 1 and 4 take the class id only).
extern "C"
{
	void (*g_d2common_fn_5E4D0)(int32_t, uint32_t) = nullptr;
	void (*g_d2common_fn_5E480)(int32_t) = nullptr;
	void (*g_d2common_fn_5D800)(int32_t, uint32_t) = nullptr;
	void (*g_d2common_fn_5E410)(int32_t) = nullptr;
}

// 1.13c D2Common+0x2f570: stores the switched left skill id and flags in a player unit's player data
// (+0x84 and +0x8C). Non-player units and null units are ignored; a player unit without player data
// is fatal 0x14A8 in retail, we return.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_2f570(uint32_t pUnit, int32_t nSkillId,
                                                int32_t nSkillFlags)
{
	D2UnitStrc* unit = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit));
	if (!unit || unit->dwUnitType != UNIT_PLAYER)
		return;

	D2PlayerDataStrc* pPlayerData = unit->pPlayerData;
	if (!pPlayerData)
		return;

	pPlayerData->nSwitchLeftSkillId = nSkillId;
	pPlayerData->nSwitchLeftSkillFlags = nSkillFlags;
}

// 1.13c D2Common+0x37470: a pair (value at data+0x4, minute at data+0x8/data+0x28) from the object
// reached through pOwner+0x4. A null pOwner is fatal 0x547 in retail; we return 0. When the minute
// divisor is zero the out minute is 0. A null data pointer reports value 0 and minute 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_37470(uint32_t pOwner, uint32_t pOutMinute)
{
	uint8_t* owner = P(pOwner);
	if (!owner)
		return 0;

	uint8_t* data = P(W(owner, 0x04));
	if (!data)
	{
		if (pOutMinute)
			W(P(pOutMinute), 0x00) = 0;
		return 0;
	}

	if (pOutMinute)
	{
		const int32_t nDivisor = static_cast<int32_t>(W(data, 0x28));
		if (nDivisor)
			W(P(pOutMinute), 0x00) = static_cast<uint32_t>(
				static_cast<int32_t>(W(data, 0x08)) / nDivisor);
		else
			W(P(pOutMinute), 0x00) = 0;
	}

	return W(data, 0x04);
}

// 1.13c D2Common+0x37660: advances the act environment's ticks and reports whether the cycle index,
// period of day or (by more than 16 minutes) the current minute moved. The minute is
// nTicks / nTimeRate and is cached in nPrev when the jump exceeds 0x10. A null pOwner is fatal 0x547
// in retail; we return 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_37660(uint32_t pOwner, int32_t nActNo)
{
	uint8_t* owner = P(pOwner);
	if (!owner)
		return 0;

	D2DrlgEnvironmentStrc* pEnvironment =
		reinterpret_cast<D2DrlgEnvironmentStrc*>(P(W(owner, 0x04)));
	if (!pEnvironment)
		return 0;

	const int32_t nOldCycleIndex = pEnvironment->nCycleIndex;
	const int32_t nOldPeriodOfDay = pEnvironment->nPeriodOfDay;

	ENVIRONMENT_UpdateTicks(pEnvironment, nActNo);

	if (pEnvironment->nTimeRate)
	{
		const int32_t nMinute = pEnvironment->nTicks / pEnvironment->nTimeRate;
		int32_t nDiff = nMinute - pEnvironment->nPrev;
		if (nDiff < 0)
			nDiff = -nDiff;
		if (nDiff > 0x10)
		{
			pEnvironment->nPrev = nMinute;
			return 1;
		}
	}

	if (nOldCycleIndex == pEnvironment->nCycleIndex
		&& nOldPeriodOfDay == pEnvironment->nPeriodOfDay)
		return 0;
	return 1;
}

// 1.13c D2Common+0x3be90: checks that a pointer array is consistent with its owner. The count at
// +0x24 must equal the count at (owner+0x10)+0x2C, and every one of the +0x24 pointers in the array
// at owner+0x00 must point at an object whose byte +0x34 has bit 0 set. Empty counts pass. A null
// owner is fatal 0x3EF in retail; we return 0 (as for a null array or element).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_3be90(uint32_t pOwner)
{
	uint8_t* owner = P(pOwner);
	if (!owner)
		return 0;

	uint8_t* pCountHolder = P(W(owner, 0x10));
	if (!pCountHolder)
		return 0;

	const uint32_t nCount = W(owner, 0x24);
	if (nCount != W(pCountHolder, 0x2C))
		return 0;
	if (nCount == 0)
		return 1;

	uint8_t* pArray = P(W(owner, 0x00));
	if (!pArray)
		return 0;

	for (uint32_t i = 0; i < nCount; ++i)
	{
		uint8_t* pEntry = P(W(pArray, static_cast<int>(i * 4)));
		if (!pEntry || (*(pEntry + 0x34) & 1) == 0)
			return 0;
	}

	return 1;
}

// 1.13c D2Common+0x5f1c0: formats a unit description into pBuffer from an explicit unit type and
// class id (the buffer's first byte is cleared). Types 0, 1, 2 and 4 dispatch to 0x5E4D0, 0x5E480,
// 0x5D800 and 0x5E410, type 3 writes "missile %d", and any other type writes "Invalid Unit Type".
// Returns pBuffer.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_5f1c0(uint32_t nUnitType, int32_t nClassId,
                                                   uint32_t pBuffer)
{
	nClassId = static_cast<int16_t>(nClassId); // retail param is a short, sign-extended
	uint8_t* buf = P(pBuffer);
	if (!buf)
		return pBuffer;

	buf[0] = 0;

	switch (nUnitType)
	{
	case 0:
		if (g_d2common_fn_5E4D0)
			g_d2common_fn_5E4D0(nClassId, pBuffer);
		break;
	case 1:
		if (g_d2common_fn_5E480)
			g_d2common_fn_5E480(nClassId);
		break;
	case 2:
		if (g_d2common_fn_5D800)
			g_d2common_fn_5D800(nClassId, pBuffer);
		break;
	case 3:
		std::sprintf(reinterpret_cast<char*>(buf), "missile %d", static_cast<int>(nClassId));
		break;
	case 4:
		if (g_d2common_fn_5E410)
			g_d2common_fn_5E410(nClassId);
		break;
	default:
		std::strcpy(reinterpret_cast<char*>(buf), "Invalid Unit Type");
		break;
	}

	return pBuffer;
}
