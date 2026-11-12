#include <ctime>
#include <D2Seed.h>
#include <D2CommonDefinitions.h>
#include <Fog.h>

//D2Common.0x6FDA5260 (#10916)
void __stdcall SEED_Return()
{
	REMOVE_LATER_Trace("SEED_Return: Useless");
}

//D2Common.0x6FDAEA80 (#10920)
// 1.13c D2Common+0x36760: three steps of the 0x19660D / 0x3C6EF35F LCG over (time32 + tick + arg), masked to 31 bits.
// d2-web: the 1.10f single-step form is replaced; time is truncated to 32 bits as retail's time32 is.
// Probe clock seam: when g_seedClockPinned is set, the test probe supplies
// the time32 that retail reads through helper 0x265D and the tick count that GetTickCount returns. Unset, the
// function reads the wall clock as before, so the game never sees the seam.
extern "C" {
	bool     g_seedClockPinned = false;
	uint32_t g_seedPinTime32 = 0;
	uint32_t g_seedPinTick = 0;
}

int __fastcall SEED_GetRandomValue(int nValue)
{
	uint32_t nTime32 = g_seedClockPinned ? g_seedPinTime32 : (uint32_t)time(NULL);
	uint32_t nTick = g_seedClockPinned ? g_seedPinTick : (uint32_t)GetTickCount();
	uint32_t nState = nTime32 + nTick + (uint32_t)nValue;
	nState = nState * 0x19660D + 0x3C6EF35F;
	nState = nState * 0x19660D + 0x3C6EF35F;
	nState = nState * 0x19660D + 0x3C6EF35F;
	return (int)(nState & 0x7FFFFFFF);
}

//D2Common.0x6FDAEAB0 (#10912)
// 1.13c D2Common+0x36750
void __fastcall SEED_InitSeed(D2SeedStrc* pSeed)
{
	pSeed->nLowSeed = 1;
	pSeed->nHighSeed = 666;
}

//D2Common.0x6FDAEAC0 (#10913)
// 1.13c D2Common+0x36740
void __fastcall SEED_InitLowSeed(D2SeedStrc* pSeed, int nLowSeed)
{
	pSeed->nLowSeed = nLowSeed;
	pSeed->nHighSeed = 666;
}

//D2Common.0x6FDAEAD0 (#10914)
uint32_t  __fastcall SEED_GetLowSeed(D2SeedStrc* pSeed)
{
	return pSeed->nLowSeed;
}

//D2Common.0x6FDAEAE0 (#10921)
void __fastcall SEED_SetSeeds(D2SeedStrc* pSeed, uint32_t  nLowSeed, uint32_t  nHighSeed)
{
	pSeed->nLowSeed = nLowSeed;
	pSeed->nHighSeed = nHighSeed;
}

//D2Common.0x6FDAEAF0 (#10922)
// 1.13c D2Common+0x36710
void __fastcall SEED_GetSeeds(D2SeedStrc* pSeed, uint32_t* pLowSeed, uint32_t* pHighSeed)
{
	*pLowSeed = pSeed->nLowSeed;
	*pHighSeed = pSeed->nHighSeed;
}

//D2Common.0x6FDAEB00 (#10915)
uint32_t __fastcall SEED_GetHighSeed(D2SeedStrc* pSeed)
{
	return pSeed->nHighSeed;
}
