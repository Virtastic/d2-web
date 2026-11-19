#pragma once

#include <D2BasicTypes.h>
#include <D2Common.h>

#pragma pack(1)

struct D2SeedStrc
{
	union
	{
		struct
		{
			uint32_t nLowSeed;					//0x00
			uint32_t nHighSeed;					//0x04
		};
		uint64_t lSeed;		//0x00
	};
};
#pragma pack()

//D2Common.0x6FDA5260 (#10916)
D2COMMON_DLL_DECL void __stdcall SEED_Return();
//D2Common.0x6FDAEA80 (#10920)
D2COMMON_DLL_DECL int __fastcall SEED_GetRandomValue(int nValue);
//D2Common.0x6FDAEAB0 (#10912)
D2COMMON_DLL_DECL void __fastcall SEED_InitSeed(D2SeedStrc* pSeed);
//D2Common.0x6FDAEAC0 (#10913)
D2COMMON_DLL_DECL void __fastcall SEED_InitLowSeed(D2SeedStrc* pSeed, int nLowSeed);
//D2Common.0x6FDAEAD0 (#10914)
D2COMMON_DLL_DECL uint32_t  __fastcall SEED_GetLowSeed(D2SeedStrc* pSeed);
//D2Common.0x6FDAEAE0 (#10921)
D2COMMON_DLL_DECL void __fastcall SEED_SetSeeds(D2SeedStrc* pSeed, uint32_t  nLowSeed, uint32_t  nHighSeed);
//D2Common.0x6FDAEAF0 (#10922)
D2COMMON_DLL_DECL void __fastcall SEED_GetSeeds(D2SeedStrc* pSeed, uint32_t* pLowSeed, uint32_t* pHighSeed);
//D2Common.0x6FDAEB00 (#10915)
D2COMMON_DLL_DECL uint32_t  __fastcall SEED_GetHighSeed(D2SeedStrc* pSeed);
//D2Common.0x6FD78E30 + Inlined at many places
inline uint64_t __fastcall SEED_RollRandomNumber(D2SeedStrc* pSeed)
{
	uint64_t lSeed = pSeed->nHighSeed + 0x6AC690C5ll * pSeed->nLowSeed;

	pSeed->lSeed = lSeed;

	return lSeed;
}

//D2Common.0x6FD7D3E0
// 1.13c D2Common+0x10B0
// 1.13c D2Common+0x1100 (the same body with nMax in ESI)
// noinline: the game callers link this one body (a single weak definition), so the coverage tool and the probes see it
inline __attribute__((noinline)) uint32_t __fastcall SEED_RollLimitedRandomNumber(D2SeedStrc* pSeed, int nMax)
{
	if (nMax > 0)
	{
		if ((nMax - 1) & nMax)
		{
			return (unsigned int)SEED_RollRandomNumber(pSeed) % nMax;
		}
		else
		{
			return SEED_RollRandomNumber(pSeed) & (nMax - 1);
		}
	}

	return 0;
}

// 1.13c D2Common+0x1180: roll in [nMin, nMax]; nMax <= nMin returns nMin without rolling, a wrapped count (sign bit) returns nMin.
inline __attribute__((noinline)) int32_t __fastcall SEED_RollRange(D2SeedStrc* pSeed, int32_t nMin, int32_t nMax)
{
	if (nMax <= nMin)
	{
		return nMin;
	}

	const uint32_t nRange = static_cast<uint32_t>(nMax - nMin);
	const uint32_t nCount = nRange + 1;
	if (static_cast<int32_t>(nCount) < 1)
	{
		return nMin;
	}

	const uint32_t nRoll = static_cast<uint32_t>(SEED_RollRandomNumber(pSeed));
	if (nCount & nRange)
	{
		return static_cast<int32_t>(static_cast<uint32_t>(nMin) + nRoll % nCount);
	}

	return static_cast<int32_t>(static_cast<uint32_t>(nMin) + (nRange & nRoll));
}

inline uint32_t SEED_RollPercentage(D2SeedStrc* pSeed)
{
	// Retail's x86 callers take the random value through EAX before the remainder.
	// Keeping the full 64-bit state here changes decisions on wasm64 even though the
	// resulting seed state remains byte-identical.
	return static_cast<uint32_t>(SEED_RollRandomNumber(pSeed)) % 100;
}
