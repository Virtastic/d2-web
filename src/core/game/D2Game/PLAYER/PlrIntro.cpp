#include "PLAYER/PlrIntro.h"

#include <algorithm>
#include <test_seam.h>

#include <Fog.h>
#include <D2BitManip.h>

#include <DataTbls/MonsterIds.h>


#pragma pack(push, 1)
struct D2NpcIndexMapStrc
{
    int32_t nNpcClassId;
    int32_t nIndex;
};
#pragma pack(pop)


constexpr D2NpcIndexMapStrc npcIndexMap[] =
{
    { -1, 0 },
    { MONSTER_GHEED, 1 },
    { MONSTER_AKARA, 2 },
    { MONSTER_KASHYA, 3 },
    { MONSTER_WARRIV1, 4 },
    { MONSTER_CHARSI, 5 },
    { MONSTER_CAIN5, 6 },
    { MONSTER_WARRIV2, 7 },
    { MONSTER_ATMA, 8 },
    { MONSTER_DROGNAN, 9 },
    { MONSTER_FARA, 10 },
    { MONSTER_LYSANDER, 11 },
    { MONSTER_GEGLASH, 12 },
    { MONSTER_MESHIF1, 13 },
    { MONSTER_JERHYN, 14 },
    { MONSTER_GREIZ, 15 },
    { MONSTER_ELZIX, 16 },
    { MONSTER_CAIN2, 17 },
    { MONSTER_CAIN3, 18 },
    { MONSTER_CAIN4, 19 },
    { MONSTER_TYRAEL1, 20 },
    { MONSTER_ASHEARA, 21 },
    { MONSTER_HRATLI, 22 },
    { MONSTER_ALKOR, 23 },
    { MONSTER_ORMUS, 24 },
    { MONSTER_IZUAL, 25 },
    { MONSTER_HALBU, 26 },
    { MONSTER_MESHIF2, 27 },
    { MONSTER_NATALYA, 28 },
    { MONSTER_LARZUK, 29 },
    { MONSTER_DREHYA, 30 },
    { MONSTER_MALAH, 31 },
    { MONSTER_NIHLATHAK, 32 },
    { MONSTER_QUAL_KEHK, 33 },
    { MONSTER_CAIN6, 34 },
};

constexpr uint32_t bufferSize = 8;

// The intro table walk every intro function shares: the bit index of the first entry naming the
// NPC class, bit 0 (the shared "any intro" bit) when none does.
static int32_t PLRINTRO_GetIntroBitIndex(int32_t nNpcId)
{
    for (int32_t i = 0; i < std::size(npcIndexMap); ++i)
    {
        if (npcIndexMap[i].nNpcClassId == nNpcId)
        {
            return npcIndexMap[i].nIndex;
        }
    }

    return 0;
}

//D2Game.0x6FC7F000
// 1.13c D2Game+0x695F0: the first matching entry's bit only; bit 0 only when no entry matches.
void __fastcall PLRINTRO_SetQuestIntroFlag(D2UnitStrc* pUnit, D2GameStrc* pGame, int32_t nNpcId)
{
    D2PlrIntroStrc* pPlayerIntro = UNITS_GetPlayerData(pUnit)->pPlayerIntro[pGame->nDifficulty];
    BITMANIP_SetBitState(pPlayerIntro->pQuestIntroFlags->pBuffer, PLRINTRO_GetIntroBitIndex(nNpcId));
}

//D2Game.0x6FC7F060
// 1.13c D2Game+0x69590
int32_t __fastcall PLRINTRO_GetQuestIntroFlag(D2UnitStrc* pUnit, D2GameStrc* pGame, int32_t nNpcId)
{
	if (D2Seam<&PLRINTRO_GetQuestIntroFlag>::hook) return D2Seam<&PLRINTRO_GetQuestIntroFlag>::hook(pUnit, pGame, nNpcId);
    D2PlrIntroStrc* pPlayerIntro = UNITS_GetPlayerData(pUnit)->pPlayerIntro[pGame->nDifficulty];
    return BITMANIP_GetBitState(pPlayerIntro->pQuestIntroFlags->pBuffer, PLRINTRO_GetIntroBitIndex(nNpcId));
}

//D2Game.0x6FC7F0C0
// not-1.13c: retail inlines this in its NPC-list loop at D2Game+0x34D00 (the first matching entry's
// bit, bit 0 only on a miss); the body here is that inlined step.
void __fastcall PLRINTRO_SetNpcIntroFlag(D2PlrIntroStrc* pPlayerIntro, int32_t nNpcId)
{
    BITMANIP_SetBitState(pPlayerIntro->pNpcIntroFlags->pBuffer, PLRINTRO_GetIntroBitIndex(nNpcId));
}

//D2Game.0x6FC7F100
// not-1.13c: retail inlines this in its NPC-list loop at D2Game+0x34C70 (the first matching entry's
// bit, bit 0 on a miss); the body here is that inlined step.
int32_t __fastcall PLRINTRO_GetNpcIntroFlag(D2PlrIntroStrc* pPlayerIntro, int32_t nNpcId)
{
    return D2_CALL(BITMANIP_GetBitState, pPlayerIntro->pNpcIntroFlags->pBuffer, PLRINTRO_GetIntroBitIndex(nNpcId));
}

//D2Game.0x6FC7F140
// 1.13c D2Game+0x69490
void __fastcall PLRINTRO_MaskNpcIntroFlag(D2GameStrc* pGame, D2UnitStrc* pPlayer, int32_t nNpcId)
{
    D2PlrIntroStrc* pPlayerIntro = UNITS_GetPlayerData(pPlayer)->pPlayerIntro[pGame->nDifficulty];
    BITMANIP_MaskBitstate(pPlayerIntro->pNpcIntroFlags->pBuffer, PLRINTRO_GetIntroBitIndex(nNpcId));
}

//D2Game.0x6FC7F1E0
// 1.13c D2Game+0x69360 (checked against decompile)
D2PlrIntroStrc* __fastcall PLRINTRO_Create(D2GameStrc* pGame)
{
    D2PlrIntroStrc* pPlayerIntro = D2_ALLOC_STRC_POOL(pGame->pMemoryPool, D2PlrIntroStrc);
    D2BitBufferStrc* pBitBuffer1 = D2_ALLOC_STRC_POOL(pGame->pMemoryPool, D2BitBufferStrc);
    uint8_t* pBuffer1 = (uint8_t*)D2_CALLOC_POOL(pGame->pMemoryPool, bufferSize);

    BITMANIP_Initialize(pBitBuffer1, pBuffer1, bufferSize);
    pPlayerIntro->pQuestIntroFlags = pBitBuffer1;

    D2BitBufferStrc* pBitBuffer2 = D2_ALLOC_STRC_POOL(pGame->pMemoryPool, D2BitBufferStrc);
    uint8_t* pBuffer2 = (uint8_t*)D2_CALLOC_POOL(pGame->pMemoryPool, bufferSize);

    BITMANIP_Initialize(pBitBuffer2, pBuffer2, bufferSize);
    pPlayerIntro->pNpcIntroFlags = pBitBuffer2;

    return pPlayerIntro;
}

// 1.13c D2Game+0x692D0
void __fastcall PLRINTRO_Destroy(D2GameStrc* pGame, D2PlrIntroStrc* pPlayerIntro)
{
    if (pPlayerIntro->pQuestIntroFlags)
    {
        D2_FREE_POOL(pGame->pMemoryPool, pPlayerIntro->pQuestIntroFlags->pBuffer);
        D2_FREE_POOL(pGame->pMemoryPool, pPlayerIntro->pQuestIntroFlags);
        pPlayerIntro->pQuestIntroFlags = nullptr;
    }

    if (pPlayerIntro->pNpcIntroFlags)
    {
        D2_FREE_POOL(pGame->pMemoryPool, pPlayerIntro->pNpcIntroFlags->pBuffer);
        D2_FREE_POOL(pGame->pMemoryPool, pPlayerIntro->pNpcIntroFlags);
        pPlayerIntro->pNpcIntroFlags = nullptr;
    }

    D2_FREE_POOL(pGame->pMemoryPool, pPlayerIntro);
}
