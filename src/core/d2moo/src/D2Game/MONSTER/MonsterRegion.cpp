#include "MONSTER/MonsterRegion.h"

#include <algorithm>

#include <D2BitManip.h>

#include <D2DataTbls.h>
#include <DataTbls/MonsterIds.h>
#include <D2Dungeon.h>
#include <DataTbls/LevelsIds.h>
#include <D2StatList.h>
#include <D2Monsters.h>

#include "GAME/Clients.h"
#include "GAME/Event.h"
#include "GAME/Targets.h"
#include "ITEMS/Items.h"
#include "MONSTER/Monster.h"
#include "MONSTER/MonsterChoose.h"
#include "MONSTER/MonsterMode.h"
#include "MONSTER/MonsterSpawn.h"
#include "MONSTER/MonsterUnique.h"
#include "QUESTS/Quests.h"
#include "QUESTS/ACT4/A4Q2.h"
#include "UNIT/SUnit.h"


// d2-web: D2Game's packed pMonReg array is not naturally aligned on wasm64.
// Bytewise volatile access preserves the native pointer without an aligned load/store,
// matching the packed unit-list accessors in SUnit.cpp.
static D2MonsterRegionStrc* MONSTERREGION_LoadPackedPointer(D2MonsterRegionStrc* const* slot)
{
    D2MonsterRegionStrc* value = nullptr;
    const volatile uint8_t* src = reinterpret_cast<const volatile uint8_t*>(slot);
    uint8_t* dst = reinterpret_cast<uint8_t*>(&value);
    for (size_t i = 0; i < sizeof(value); ++i) dst[i] = src[i];
    return value;
}

static void MONSTERREGION_StorePackedPointer(D2MonsterRegionStrc** slot, D2MonsterRegionStrc* value)
{
    const uint8_t* src = reinterpret_cast<const uint8_t*>(&value);
    volatile uint8_t* dst = reinterpret_cast<volatile uint8_t*>(slot);
    for (size_t i = 0; i < sizeof(value); ++i) dst[i] = src[i];
}

// 1.13c D2Game+0x46C30 (1.10f had this block inline in sub_6FC66260): the subtile bounds the roll adds
// to / takes the modulus of. With a coord list: pBox[1] through DUNGEON_GameTileToSubtileCoords. Else
// the room's own coords. 1.13c passes the coord list in EAX, the room and &nRight on the stack and the
// other three outs in EBX/ESI/EDI; here they are ordinary parameters.
void __fastcall sub_46C30(D2ActiveRoomStrc* pRoom, D2RoomCoordListStrc* pRoomCoordList, int32_t* pLeft, int32_t* pTop, int32_t* pRight, int32_t* pBottom)
{
    if (pRoomCoordList)
    {
        D2DrlgCoordStrc drlgCoord = {};
        memcpy(&drlgCoord, &pRoomCoordList->pBox[1], sizeof(drlgCoord));
        DUNGEON_GameTileToSubtileCoords(&drlgCoord.nPosX, &drlgCoord.nPosY);
        DUNGEON_GameTileToSubtileCoords(&drlgCoord.nWidth, &drlgCoord.nHeight);
        *pLeft = drlgCoord.nPosX + 1;
        *pTop = drlgCoord.nPosY + 1;
        *pRight = drlgCoord.nWidth - (drlgCoord.nPosX + 1);
        *pBottom = drlgCoord.nHeight - (drlgCoord.nPosY + 1);
    }
    else
    {
        D2DrlgCoordsStrc drlgCoords = {};
        DUNGEON_GetRoomCoordinates(pRoom, &drlgCoords);
        *pLeft = drlgCoords.nSubtileX + 1;
        *pTop = drlgCoords.nSubtileY + 1;
        *pRight = drlgCoords.nSubtileWidth - 1;
        *pBottom = drlgCoords.nSubtileHeight - 1;
    }
}

// 1.13c D2Game+0x46B20 (1.10f had this block inline in sub_6FC66260; 0x47450 is its only caller):
// 1 when (nX,nY) lies within the level's WarpDist of a warp of the room, or of the act's start
// location (11 = the spawn-type DUNGEON_FindActSpawnLocation is asked for).
// d2-web: the five D2Common lookups are the FN replay's seams (null in the game).
D2Spawn46B20Seam g_d2Spawn46B20;

int32_t __fastcall sub_46B20(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nX, int32_t nY)
{
    if (!pRoom)
    {
        return 0;
    }

    const int32_t nLevelId = g_d2Spawn46B20.levelId ? g_d2Spawn46B20.levelId(pRoom) : DUNGEON_GetLevelIdFromRoom(pRoom);
    if (!nLevelId)
    {
        return 0;
    }

    int32_t* pWarpCoordinates = g_d2Spawn46B20.warpCoordinates ? g_d2Spawn46B20.warpCoordinates(pRoom) : DUNGEON_GetWarpCoordinatesFromRoom(pRoom);
    if (!pWarpCoordinates)
    {
        return 0;
    }

    const D2LevelsTxt* pLevelsTxt = g_d2Spawn46B20.levelsRecord ? g_d2Spawn46B20.levelsRecord(nLevelId) : DATATBLS_GetLevelsTxtRecord(nLevelId);
    const int32_t nWarpDist = pLevelsTxt->dwWarpDist;
    for (int32_t j = 0; j < pWarpCoordinates[18]; ++j)
    {
        const int32_t nXDiff = nX - pWarpCoordinates[j];
        const int32_t nYDiff = nY - pWarpCoordinates[j + 9];
        if (nXDiff * nXDiff + nYDiff * nYDiff < nWarpDist)
        {
            return 1;
        }
    }

    const uint8_t nActNo = g_d2Spawn46B20.actNo ? g_d2Spawn46B20.actNo(nLevelId) : DRLG_GetActNoFromLevelId(nLevelId);
    D2DrlgActStrc* const pAct = pGame->pAct[nActNo];

    int32_t nSpawnX = 0;
    int32_t nSpawnY = 0;
    if (g_d2Spawn46B20.actSpawnLocation)
    {
        g_d2Spawn46B20.actSpawnLocation(pAct, nLevelId, 11, &nSpawnX, &nSpawnY);
    }
    else
    {
        DUNGEON_FindActSpawnLocation(pAct, nLevelId, 11, &nSpawnX, &nSpawnY);
    }
    if (nSpawnX > 0 && nSpawnY > 0)
    {
        DUNGEON_GameTileToSubtileCoords(&nSpawnX, &nSpawnY);
        const int32_t nXDiff = nX - nSpawnX;
        const int32_t nYDiff = nY - nSpawnY;
        if (nXDiff * nXDiff + nYDiff * nYDiff < nWarpDist)
        {
            return 1;
        }
    }

    return 0;
}

// 1.13c D2Game+0x47450 callee seams; both null in the game
D2Spawn47450Seam g_d2Spawn47450;

//1.10f:D2Game.0x6FC66260
// 1.13c D2Game+0x47450
int32_t __fastcall sub_6FC66260(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, D2RoomCoordListStrc* pRoomCoordList, int32_t nSuperUniqueId, int32_t* pX, int32_t* pY, int32_t a7)
{
    int32_t nLeft = 0;
    int32_t nTop = 0;
    int32_t nRight = 0;
    int32_t nBottom = 0;

    sub_46C30(pRoom, pRoomCoordList, &nLeft, &nTop, &nRight, &nBottom);

    int32_t nRoomCoordListIndex = 0;
    if (pRoomCoordList)
    {
        nRoomCoordListIndex = pRoomCoordList->nIndex;
    }

    D2SeedStrc* pSeed = nullptr;
    if (pRoom)
    {
        pSeed = &pRoom->pSeed;
    }

    for (int32_t i = 0; i < 20; ++i)
    {
        const int32_t nX = ITEMS_RollLimitedRandomNumber(pSeed, nRight) + nLeft;
        const int32_t nY = ITEMS_RollLimitedRandomNumber(pSeed, nBottom) + nTop;

        // d2-web: retail (1.13c D2Game 0x6FC66B20, called from 0x6FC67450) REJECTS a spot within
        // the level's WarpDist of any warp or of the act's start location; the upstream decompile
        // had the test the other way round (and a `break` that left the search loop), which left
        // every level empty. Written from the 1.13c listing; 1.13c has it as the callee sub_46B20.
        if (a7 && pRoom && (g_d2Spawn47450.warpCheck ? g_d2Spawn47450.warpCheck(pGame, pRoom, nX, nY) : sub_46B20(pGame, pRoom, nX, nY)))
        {
            continue;
        }

        const int32_t bSpawnMonster = 1;
#ifdef D2_MONS_TRACE
        if (DUNGEON_GetLevelIdFromRoom(pRoom) == d2_trace_level) fprintf(stderr, "  try %d: (%d,%d) seed-after %u/%u box %d,%d-%d,%d coordlist %p idx %d tile %d\n", i, nX, nY, pSeed->nLowSeed, pSeed->nHighSeed, nLeft, nTop, nRight, nBottom, (void*)pRoomCoordList, nRoomCoordListIndex, pRoomCoordList ? D2Common_10095(pRoom, nX, nY) : -1);
#endif
        if (bSpawnMonster)
        {
            // 1.13c D2Game 0x47450 builds the create-struct inline and calls 0xEEF10 directly (the
            // two 1.10f wrappers sub_6FC6A030 / D2GAME_SpawnMonster_6FC69F10 folded in here).
            D2UnkMonCreateStrc monCreate = {};
            monCreate.pGame = pGame;
            monCreate.pRoom = pRoom;
            monCreate.nMonsterId = nSuperUniqueId;
            monCreate.nAnimMode = 1;
            monCreate.nX = nX;
            monCreate.nY = nY;
            monCreate.field_20 = -1;
            monCreate.nFlags = 1;
            if (pRoomCoordList)
            {
                if (D2Common_10095(pRoom, nX, nY) != nRoomCoordListIndex)
                {
                    continue;
                }
                monCreate.pRoomCoordList = pRoomCoordList;
            }

            if ((g_d2Spawn47450.spawn ? g_d2Spawn47450.spawn(&monCreate) : D2GAME_SpawnNormalMonster_6FC68E30(&monCreate)) != nullptr)
            {
                *pX = nX;
                *pY = nY;
                return 1;
            }
        }
    }

    return 0;
}

// 1.13c D2Game+0x47CF0 callee seams; all null in the game
D2Spawn47CF0Seam g_d2Spawn47CF0;

static int32_t s47cf0_levelId(D2ActiveRoomStrc* pRoom) { return g_d2Spawn47CF0.levelId ? g_d2Spawn47CF0.levelId(pRoom) : DUNGEON_GetLevelIdFromRoom(pRoom); }
static int32_t s47cf0_questCheck(D2GameStrc* pGame, int32_t nQuest) { return g_d2Spawn47CF0.questCheck ? g_d2Spawn47CF0.questCheck(pGame, nQuest) : QUESTS_CheckNotIntroQuest(pGame, nQuest); }
static D2UnitStrc* s47cf0_spawnSupUniq(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nX, int32_t nY, int32_t nSuperUniqueId) { return g_d2Spawn47CF0.spawnSuperUnique ? g_d2Spawn47CF0.spawnSuperUnique(pGame, pRoom, nX, nY, nSuperUniqueId) : D2GAME_SpawnSuperUnique_6FC6F690(pGame, pRoom, nX, nY, nSuperUniqueId); }
static D2UnitStrc* s47cf0_spawnMon(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nX, int32_t nY, int32_t nMonsterId, int32_t nAnimMode, int32_t a7, int16_t nFlags) { return g_d2Spawn47CF0.spawnMonster ? g_d2Spawn47CF0.spawnMonster(pGame, pRoom, nX, nY, nMonsterId, nAnimMode, a7, nFlags) : D2GAME_SpawnMonster_6FC69F10(pGame, pRoom, nX, nY, nMonsterId, nAnimMode, a7, nFlags); }
static int32_t s47cf0_randomId(D2ActiveRoomStrc* pRoom, int32_t nMonsterId) { return g_d2Spawn47CF0.randomId ? g_d2Spawn47CF0.randomId(pRoom, nMonsterId) : D2Common_11063(pRoom, nMonsterId); }
// the four callees only the 0x46F70 regular path reaches
static uint32_t s47cf0_checkFlag(int32_t nMonsterId, int32_t nFlag) { return g_d2Spawn47CF0.checkMonStats2Flag ? g_d2Spawn47CF0.checkMonStats2Flag(nMonsterId, nFlag) : MONSTERREGION_CheckMonStats2Flag(nMonsterId, nFlag); }
static void s47cf0_allocUnit(int32_t nUnitType, int32_t nClassId, int32_t nX, int32_t nY, D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, char a7, int32_t nMode, uint32_t a3) { if (g_d2Spawn47CF0.allocUnit) g_d2Spawn47CF0.allocUnit(nUnitType, nClassId, nX, nY, pGame, pRoom, a7, nMode, a3); else SUNIT_AllocUnitData(nUnitType, nClassId, nX, nY, pGame, pRoom, a7, nMode, a3); }
static int32_t s47cf0_unitX(D2UnitStrc* pUnit) { return g_d2Spawn47CF0.unitX ? g_d2Spawn47CF0.unitX(pUnit) : CLIENTS_GetUnitX(pUnit); }
static int32_t s47cf0_unitY(D2UnitStrc* pUnit) { return g_d2Spawn47CF0.unitY ? g_d2Spawn47CF0.unitY(pUnit) : CLIENTS_GetUnitY(pUnit); }

// 1.13c D2Game+0x46F70: the regular (MonStats) path, outlined by 1.13c LTCG out of
// D2GAME_SpawnPresetMonster_6FC66560. Convention: EDI = nClassId, stack (ret 0x14) = pGame, pRoom,
// nX, nY, nMode; result in EAX. Body is the 1.10f inline block, unchanged except that the critter test
// is the callee 0x13B0 (CheckMonStats2Flag(nClassId, 0xd)) rather than a direct MonStats2 record read,
// which is the same test. 
D2UnitStrc* __fastcall D2GAME_SpawnRegularMonster_6FC66F70(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nClassId, int32_t nX, int32_t nY, int32_t nMode)
{
    if (nClassId < 0 || nClassId >= sgptDataTables->nMonStatsTxtRecordCount)
    {
        return nullptr;
    }

    D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(nClassId);
    if (!pMonStatsTxtRecord)
    {
        return nullptr;
    }

    if (nClassId == MONSTER_PRISONDOOR)
    {
        --nX;

        if (!s47cf0_questCheck(pGame, QUEST_A5Q2_RESCUESOLDIERS))
        {
            nMode = MONMODE_DEAD;
        }
    }

    if (s47cf0_checkFlag(nClassId, MONSTATS2FLAGINDEX_CRITTER))
    {
        return nullptr;
    }

    int16_t nFlags = 0;
    if (pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_NEVERCOUNT])
    {
        nFlags = 8;
    }

    D2UnitStrc* pMonster = s47cf0_spawnMon(pGame, pRoom, nX, nY, nClassId, nMode, -1, nFlags);
    if (!pMonster)
    {
        if (!sub_6FC670A0(nClassId))
        {
            return nullptr;
        }

        pMonster = s47cf0_spawnMon(pGame, pRoom, nX, nY, nClassId, nMode, 4, nFlags);
        if (!pMonster)
        {
            return nullptr;
        }
    }

    if (s47cf0_checkFlag(nClassId, MONSTATS2FLAGINDEX_OBJCOL))
    {
        if (nClassId == MONSTER_BARRICADEDOOR1)
        {
            s47cf0_allocUnit(UNIT_OBJECT, 571, s47cf0_unitX(pMonster), s47cf0_unitY(pMonster), pGame, pRoom, 1, 0, 0);
        }
        else if (nClassId == MONSTER_BARRICADEDOOR2)
        {
            s47cf0_allocUnit(UNIT_OBJECT, 572, s47cf0_unitX(pMonster), s47cf0_unitY(pMonster), pGame, pRoom, 1, 0, 0);
        }
    }

    return pMonster;
}

//D2Game.0x6FC66560
// 1.13c D2Game+0x47CF0: the regular path is outlined at 0x46F70, the 24/26 group spawn at 0x470F0,
// the fallen/shaman id chain at 0x46E80, the champion minion loop at 0x47880 and the 10/11 chain at
// 0x1D30; both D2Common_11063 call sites (thunk 0xADFC) are wrapped in SanitizeMonsterId (0x1850),
// which is the identity for every id these two sites can carry. The body is otherwise the 1.10f body,
// plus the D2Spawn47CF0Seam indirection above (all seam members are null in the game). See
D2UnitStrc* __fastcall D2GAME_SpawnPresetMonster_6FC66560(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nClassId, int32_t nX, int32_t nY, int32_t nMode)
{
    if (nClassId < 0)
    {
        return 0;
    }

    int32_t bRegularId = 1;
    if (nClassId >= sgptDataTables->nMonStatsTxtRecordCount)
    {
        const int32_t nSuperUniqueId = nClassId - sgptDataTables->nMonStatsTxtRecordCount;
        if (nSuperUniqueId < sgptDataTables->nSuperUniquesTxtRecordCount)
        {
            return s47cf0_spawnSupUniq(pGame, pRoom, nX, nY, nSuperUniqueId);
        }

        nClassId = nSuperUniqueId - sgptDataTables->nSuperUniquesTxtRecordCount;
        bRegularId = 0;
    }

    const int32_t nLevelId = s47cf0_levelId(pRoom);

    if (bRegularId == 1)
    {
        if (nClassId == MONSTER_CATAPULT2)
        {
            if (s47cf0_questCheck(pGame, QUEST_A5Q1_SHENK) && s47cf0_levelId(pRoom) == LEVEL_BLOODYFOOTHILLS)
            {
                nClassId = MONSTER_CATAPULT3;
            }
        }
        else if (nClassId == MONSTER_CATAPULTSPOTTER2)
        {
            if (s47cf0_questCheck(pGame, QUEST_A5Q1_SHENK) && s47cf0_levelId(pRoom) == LEVEL_BLOODYFOOTHILLS)
            {
                nClassId = MONSTER_CATAPULTSPOTTER3;
            }
        }

        if (pGame->nDifficulty != DIFFMODE_NORMAL && (nClassId == MONSTER_MINION1 || nClassId == MONSTER_DEATHMAULER1) && nLevelId == LEVEL_BLOODYFOOTHILLS)
        {
            return nullptr;
        }

        return D2GAME_SpawnRegularMonster_6FC66F70(pGame, pRoom, nClassId, nX, nY, nMode);
    }

    switch (nClassId)
    {
    case 2:
    {
        D2MonStatsTxt* pMonStatsTxtRecord = nullptr;
        const int32_t nMonsterId = MONSTERCHOOSE_GetPresetMonsterId(pGame, MONSTERREGION_GetMonsterRegionFromLevelId(pGame->pMonReg, DUNGEON_GetLevelIdFromPopulatedRoom(pRoom)), pRoom, &pMonStatsTxtRecord, 0, 1);
        if (pMonStatsTxtRecord)
        {
            sub_6FC6E8D0(pGame, pRoom, nullptr, nMonsterId, 0, 0, 0, 1);
        }
        return nullptr;
    }
    case 3:
    {
        D2MonStatsTxt* pMonStatsTxtRecord = nullptr;
        const int32_t nMonsterId = MONSTERCHOOSE_GetPresetMonsterId(pGame, MONSTERREGION_GetMonsterRegionFromLevelId(pGame->pMonReg, DUNGEON_GetLevelIdFromPopulatedRoom(pRoom)), pRoom, &pMonStatsTxtRecord, 0, 1);
        if (!pMonStatsTxtRecord)
        {
            return nullptr;
        }

        D2UnitStrc* pMonster = s47cf0_spawnMon(pGame, pRoom, nX, nY, nMonsterId, nMode, -1, 0);
        if (!pMonster)
        {
            return nullptr;
        }

        D2GAME_MONSTERS_Unk_6FC6FFD0(pGame, pMonster, MONUMOD_CHAMPMODS);

        if (!MONSTERUNIQUE_CheckMonTypeFlag(pMonster, MONTYPEFLAG_CHAMPION))
        {
            return pMonster;
        }

        const int32_t nMinions = ITEMS_RollRandomNumber(&pMonster->pSeed) % 3 + 1;
        for (int32_t i = 0; i < nMinions; ++i)
        {
            D2UnitStrc* pMinion = sub_6FC69F70(pGame, 0, pMonster, nMonsterId, 1, 4, 0);
            if (pMinion)
            {
                D2GAME_MONSTERS_Unk_6FC6FFD0(pGame, pMinion, MONUMOD_CHAMPMODS);
            }
        }
        return pMonster;
    }
    case 4:
    {
        return s47cf0_spawnMon(pGame, pRoom, nX, nY, MONSTER_NAVI, nMode, -1, 0);
    }
    case 5:
    {
        return s47cf0_spawnMon(pGame, pRoom, nX, nY, MONSTER_BLOODRAVEN, nMode, -1, 0);
    }
    case 8:
    {
        return s47cf0_spawnMon(pGame, pRoom, nX, nY, MONSTER_MAGGOTQUEEN1, nMode, -1, 8);
    }
    case 10:
    case 11:
    {
        int16_t nFlags = 0;
        if (nClassId == 10)
        {
            nFlags = 4;
        }

        int32_t nChainIncrements = 0;
        switch (nLevelId)
        {
        case LEVEL_SPIDERFOREST:
        case LEVEL_GREATMARSH:
        case LEVEL_FLAYERJUNGLE:
        {
            nChainIncrements = 1;
            break;
        }
        case LEVEL_SEWERSA3LEV1:
        case LEVEL_SEWERSA3LEV2:
        {
            nChainIncrements = 2;
            break;
        }
        case LEVEL_SWAMPYPITLEV1:
        case LEVEL_SWAMPYPITLEV2:
        case LEVEL_FLAYERDUNGEONLEV1:
        case LEVEL_FLAYERDUNGEONLEV2:
        case LEVEL_SWAMPYPITLEV3:
        case LEVEL_FLAYERDUNGEONLEV3:
        {
            nChainIncrements = 0;
            break;
        }
        default:
            break;
        }

        int32_t nMonsterId = -1;
        D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(261);
        if (pMonStatsTxtRecord)
        {
            nMonsterId = pMonStatsTxtRecord->nBaseId;
            for (int32_t i = 0; i < nChainIncrements; ++i)
            {
                nMonsterId = pMonStatsTxtRecord->nNextInClass;
                pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(pMonStatsTxtRecord->nNextInClass);
                if (!pMonStatsTxtRecord)
                {
                    break;
                }
            }
        }

        return s47cf0_spawnMon(pGame, pRoom, nX, nY, nMonsterId, nMode, -1, nFlags);
    }
    case 17:
    case 18:
    case 22:
    case 23:
    case 25:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    {
        int32_t nMonsterId = 0;
        switch (nClassId)
        {
        case 25:
        case 27:
        case 28:
            return nullptr;
        case 17:
            nMonsterId = MONSTER_FALLEN1;
            break;
        case 18:
            nMonsterId = MONSTER_FALLENSHAMAN1;
            break;
        case 22:
            nMonsterId = MONSTER_FETISH1;
            break;
        case 23:
            nMonsterId = MONSTER_FETISHSHAMAN1;
            break;
        case 29:
            nMonsterId = MONSTER_MINION1;
            nMode = MONMODE_DEAD;
            break;
        case 30:
            if (nLevelId == LEVEL_BLOODYFOOTHILLS)
            {
                nMonsterId = MONSTER_DEATHMAULER1;
            }
            else
            {
                nMonsterId = MONSTER_IMP1;
            }
            nMode = MONMODE_DEAD;
            break;
        case 31:
            nMonsterId = MONSTER_ACT5BARB1;
            nMode = MONMODE_DEAD;
            break;
        case 32:
            nMonsterId = MONSTER_REANIMATEDHORDE3;
            nMode = MONMODE_DEAD;
            break;
        default:
            break;
        }

        nMonsterId = s47cf0_randomId(pRoom, MONSTERREGION_SanitizeMonsterId(nMonsterId));
        int32_t nBaseId = nMonsterId;
        D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(nMonsterId);
        if (pMonStatsTxtRecord)
        {
            nBaseId = pMonStatsTxtRecord->nBaseId;
        }

        if (nBaseId == MONSTER_FALLEN1)
        {
            switch (nLevelId)
            {
            case LEVEL_BLACKMARSH:
                nMonsterId = MONSTER_FALLEN2;
                break;
            case LEVEL_TAMOEHIGHLAND:
            case LEVEL_PITLEV1:
            case LEVEL_PITLEV2:
                nMonsterId = MONSTER_FALLEN3;
                break;
            default:
                break;
            }
        }
        else if (nBaseId == MONSTER_FALLENSHAMAN1)
        {
            switch (nLevelId)
            {
            case LEVEL_BLACKMARSH:
            case LEVEL_TAMOEHIGHLAND:
                nMonsterId = MONSTER_FALLENSHAMAN2;
                break;
            case LEVEL_PITLEV1:
            case LEVEL_PITLEV2:
                nMonsterId = MONSTERREGION_SanitizeMonsterId(MONSTER_FALLENSHAMAN3);
                break;
            default:
                break;
            }
        }

        pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(nMonsterId);
        int16_t nFlags = 0;
        if (pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_NEVERCOUNT])
        {
            nFlags = 8;
        }

        D2UnitStrc* pMonster = s47cf0_spawnMon(pGame, pRoom, nX, nY, nMonsterId, nMode, -1, nFlags);
        if (!pMonster)
        {
            pMonster = s47cf0_spawnMon(pGame, pRoom, nX, nY, nMonsterId, nMode, 4, nFlags);
            if (!pMonster)
            {
                return nullptr;
            }
        }

        if (nMonsterId == MONSTER_REANIMATEDHORDE3)
        {
            EVENT_SetEvent(pGame, pMonster, EVENTTYPE_MONUMOD, pGame->dwGameFrame + ITEMS_RollRandomNumber(&pMonster->pSeed) % 50 + 250, 0, 0);
        }

        if (!pMonster || nMode != MONMODE_DEAD)
        {
            return pMonster;
        }

        pMonster->dwFlags |= UNITFLAG_ISRESURRECT;
        return pMonster;
    }
    case 24:
    case 26:
    {
        if (pGame->nDifficulty != DIFFMODE_NORMAL)
        {
            return nullptr;
        }

        int32_t nMonsterId = 0;
        if (nClassId == 24)
        {
            if (nLevelId == LEVEL_BLOODYFOOTHILLS)
            {
                nMonsterId = MONSTER_DEATHMAULER1;
            }
            else
            {
                nMonsterId = MONSTER_IMP1;
            }
        }
        else if (nClassId == 26)
        {
            nMonsterId = MONSTER_MINION1;
        }

        nMonsterId = s47cf0_randomId(pRoom, MONSTERREGION_SanitizeMonsterId(nMonsterId));
        D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(nMonsterId);
        
        const uint8_t nMaxGrp = pMonStatsTxtRecord->nMaxGrp;
        const uint8_t nMinGrp = pMonStatsTxtRecord->nMinGrp;
        if (nMinGrp && nMaxGrp && nMaxGrp >= nMinGrp)
        {
            D2RoomCoordListStrc* pRoomCoordList = D2Common_10096(pRoom, nX, nY);
            if (pRoomCoordList)
            {
                D2UnitStrc* pMonster = sub_6FC6A030(pGame, pRoom, pRoomCoordList, nX, nY, nMonsterId, 1, -1, 0);
                if (pMonster)
                {
                    const int32_t nMinions = nMinGrp + ITEMS_RollLimitedRandomNumber(&pMonster->pSeed, nMaxGrp - nMinGrp + 1) - 1;
                    for (int32_t i = 0; i < nMinions; ++i)
                    {
                        sub_6FC69F70(pGame, pRoomCoordList, pMonster, nMonsterId, 1, 3, 0);
                    }
                }
            }
        }
        break;
    }
    default:
        break;
    }

    return nullptr;
}

//D2Game.0x6FC66FC0
// 1.13c D2Game+0x1320
D2MonStats2Txt* __fastcall MONSTERREGION_GetMonStats2TxtRecord(int32_t nMonsterId)
{
    if (nMonsterId >= 0 && nMonsterId < sgptDataTables->nMonStatsTxtRecordCount)
    {
        const int32_t nMonStatsEx = sgptDataTables->pMonStatsTxt[nMonsterId].wMonStatsEx;
        if (nMonStatsEx >= 0 && nMonStatsEx < sgptDataTables->nMonStats2TxtRecordCount)
        {
            return &sgptDataTables->pMonStats2Txt[nMonStatsEx];
        }
    }

    return nullptr;
}

//D2Game.0x6FC67010
// 1.13c D2Game+0x13B0: same body (id range, wMonStatsEx, MonStats2 flag byte/bit), EAX = id, EDX = flag
uint32_t __fastcall MONSTERREGION_CheckMonStats2Flag(int32_t nMonsterId, int32_t nFlag)
{
    D2MonStats2Txt* pMonStats2TxtRecord = MONSTERREGION_GetMonStats2TxtRecord(nMonsterId);

    if (pMonStats2TxtRecord)
    {
        return pMonStats2TxtRecord->nFlags[nFlag >> 3] & gdwBitMasks[nFlag & 7];
    }

    return 0;
}

//D2Game.0x6FC67080
// 1.13c D2Game+0x1850: same body (range check -> id or -1; 24 bytes)
int32_t __fastcall MONSTERREGION_SanitizeMonsterId(int32_t nMonsterId)
{
    if (nMonsterId >= 0 && nMonsterId < sgptDataTables->nMonStatsTxtRecordCount)
    {
        return nMonsterId;
    }

    return -1;
}

//D2Game.0x6FC670A0
// 1.13c D2Game+0x46D90: same sanitize-then-switch body (0xE5, 0x11C..0x120, 0x188, 0x189 -> 0); 
int32_t __fastcall sub_6FC670A0(int32_t nMonsterId)
{
    switch (MONSTERREGION_SanitizeMonsterId(nMonsterId))
    {
    case MONSTER_RADAMENT:
    case MONSTER_MAGGOTQUEEN1:
    case MONSTER_MAGGOTQUEEN2:
    case MONSTER_MAGGOTQUEEN3:
    case MONSTER_MAGGOTQUEEN4:
    case MONSTER_MAGGOTQUEEN5:
    case MONSTER_WINDOW1:
    case MONSTER_WINDOW2:
        return 0;
	default:
		return 1;
    }
}

// 1.13c D2Game+0x47A20 replay seams (kind 3450, s3252): each member stands in for one retail callee when set; all null in the game.
D2Populate47A20Seam g_d2Pop47A20 = {};
D2Gate46CD0Seam g_d2Gate46CD0 = {};

// 1.13c D2Game+0x46CD0 (register convention EDI = room, EBX = game): the region/level/density gate that opens 0x47A20
static int32_t sub_46CD0(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom)
{
    const D2Gate46CD0Seam& z = g_d2Gate46CD0;
    D2MonsterRegionStrc* pMonsterRegion = MONSTERREGION_GetMonsterRegionFromLevelId(pGame->pMonReg, z.roomLevel ? z.roomLevel(pRoom) : DUNGEON_GetLevelIdFromRoom(pRoom));
    if (!pMonsterRegion)
    {
        return 0;
    }

    ++pMonsterRegion->unk0x04;

    const int32_t nLevelId = z.populatedLevel ? z.populatedLevel(pRoom) : DUNGEON_GetLevelIdFromPopulatedRoom(pRoom);
    if (!nLevelId)
    {
        return 0;
    }

    if (pMonsterRegion->unk0x0C < 0)
    {
        pMonsterRegion->unk0x0C = z.countRooms ? z.countRooms(pGame->pAct[pMonsterRegion->nAct], nLevelId) : DUNGEON_GetNumberOfPopulatedRoomsInLevel(pGame->pAct[pMonsterRegion->nAct], nLevelId);
        // d2-web diagnosis (not retail): which of the level's rooms read as unpopulated when the count was taken.
        if (!z.countRooms) { extern void d2_note_popzero(D2DrlgActStrc*, int32_t); d2_note_popzero(pGame->pAct[pMonsterRegion->nAct], nLevelId); }
    }

#ifdef D2_MONS_TRACE
    fprintf(stderr, "populate: level %d monden %u rooms %d\n", nLevelId, pMonsterRegion->dwMonDen, pMonsterRegion->unk0x0C);
#endif
    return pMonsterRegion->dwMonDen && pMonsterRegion->unk0x0C && !((z.roomLevel ? z.roomLevel(pRoom) : DUNGEON_GetLevelIdFromRoom(pRoom)) == LEVEL_CHAOSSANCTUM && (z.chaosCleared ? z.chaosCleared(pGame) : ACT4Q2_IsChaosSanctumCleared(pGame)));
}

int32_t D2GAME_Gate46CD0(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom) { return sub_46CD0(pGame, pRoom); }   // replay entry (kind 3451, s3253)

// 1.13c D2Game+0x479B0: champion group (unit, its 1-3 minions)
static int32_t sub_479B0(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, D2RoomCoordListStrc* pRoomCoordList, int32_t nChampionId)
{
    D2UnitStrc* pChampion = sub_6FC6E8D0(pGame, pRoom, pRoomCoordList, nChampionId, 1, 0, 0, 1);
    if (!pChampion)
    {
        return 0;
    }

    if (MONSTERUNIQUE_CheckMonTypeFlag(pChampion, MONTYPEFLAG_CHAMPION))
    {
        for (int32_t j = ITEMS_RollLimitedRandomNumber(&pChampion->pSeed, 3) + 1; j > 0; --j)
        {
            D2UnitStrc* pMinion = sub_6FC69F70(pGame, pRoomCoordList, pChampion, nChampionId, 1, 4, 0);
            if (pMinion)
            {
                D2GAME_MONSTERS_Unk_6FC6FFD0(pGame, pMinion, MONUMOD_CHAMPMODS);
            }
        }
    }

    return 1;
}

// 1.13c D2Game+0x47900: boss placement + spawn
static int32_t sub_47900(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nMonsterId, D2UnkMonCreateStrc2* v24)
{
    int32_t nX = 0;
    int32_t nY = 0;
    return sub_6FC67570(pGame, pRoom, nullptr, nMonsterId, v24, &nX, &nY) && sub_6FC6A350(pGame, pRoom, nX, nY, nullptr, nMonsterId, 1, v24, 0);
}

// 1.13c D2Game+0x46D40: the class (through its base class) is monster 0x13 or 0x5B (fallen / scarab)
static int32_t sub_46D40(int32_t nMonsterId)
{
    const D2MonStatsTxt* pRecord = MONSTERMODE_GetMonStatsTxtRecord(nMonsterId);
    const int32_t nBaseId = pRecord ? pRecord->nBaseId : nMonsterId;
    return nBaseId == MONSTER_FALLEN1 || nBaseId == MONSTER_SCARAB1;
}

//D2Game.0x6FC67190
// 1.13c D2Game+0x47A20
void __fastcall D2GAME_PopulateRoom_6FC67190(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom)
{
    const D2Populate47A20Seam& z = g_d2Pop47A20;
    if (!(z.gate ? z.gate(pGame, pRoom) : sub_46CD0(pGame, pRoom)))
    {
        return;
    }

    D2RoomCoordListStrc* pRoomCoordList = z.coordList ? z.coordList(pRoom) : DUNGEON_GetRoomCoordList(pRoom);
    if (!pRoomCoordList)
    {
        return;
    }

    D2MonsterRegionStrc* pMonsterRegion = MONSTERREGION_GetMonsterRegionFromLevelId(pGame->pMonReg, z.levelId ? z.levelId(pRoom) : DUNGEON_GetLevelIdFromPopulatedRoom(pRoom));
    if (pMonsterRegion->dwMonDen > 10000)
    {
        pMonsterRegion->dwMonDen = 10000;
    }

    int32_t bIncrease = 0;
    for (; pRoomCoordList; pRoomCoordList = pRoomCoordList->pNext)
    {
        if (!pRoomCoordList->nIndex || pRoomCoordList->bNode)
        {
            continue;
        }

        D2DrlgCoordStrc drlgCoord = {};
        memcpy(&drlgCoord, &pRoomCoordList->pBox[1], sizeof(D2DrlgCoordStrc));
        if (!drlgCoord.nPosX && !drlgCoord.nWidth)
        {
            continue;
        }

        DUNGEON_GameTileToSubtileCoords(&drlgCoord.nPosX, &drlgCoord.nPosY);
        DUNGEON_GameTileToSubtileCoords(&drlgCoord.nWidth, &drlgCoord.nHeight);

        for (int32_t i = ((drlgCoord.nWidth - drlgCoord.nPosX) / 3) * ((drlgCoord.nHeight - drlgCoord.nPosY) / 3); i > 0; --i)   // d2-web: 1.13c multiplies the two quotients (0x6FC67AE7)
        {
            if ((ITEMS_RollRandomNumber(&pGame->pGameSeed) % 100000) > pMonsterRegion->dwMonDen)
            {
                continue;
            }

            D2MonStatsTxt* pMonStatsTxtRecord = nullptr;
            const int32_t nMonsterId = z.preset ? z.preset(pGame, pMonsterRegion, pRoom, &pMonStatsTxtRecord, 20, 0) : MONSTERCHOOSE_GetPresetMonsterId(pGame, pMonsterRegion, pRoom, &pMonStatsTxtRecord, 20u, 0);
            if (!pMonStatsTxtRecord)
            {
                return;
            }

            int32_t nType = z.bossType ? z.bossType(pMonsterRegion, pRoom) : MONSTERCHOOSE_GetBossSpawnType(pMonsterRegion, pRoom);
            if (nType == 1)
            {
                nType = 2;
            }

            int32_t bSpawned;
            if (nType == 0)
            {
                const int32_t nChampionId = z.preset ? z.preset(pGame, pMonsterRegion, pRoom, &pMonStatsTxtRecord, 0, 1) : MONSTERCHOOSE_GetPresetMonsterId(pGame, pMonsterRegion, pRoom, &pMonStatsTxtRecord, 0, 1);
                bSpawned = z.champion ? z.champion(pGame, pRoom, pRoomCoordList, nChampionId) : sub_479B0(pGame, pRoom, pRoomCoordList, nChampionId);
            }
            else if (nType == 1)   // unreachable (1 became 2 above), as in retail
            {
                D2UnkMonCreateStrc2* v24 = sub_6FC62640(pRoom);
                bSpawned = z.boss ? z.boss(pGame, pRoom, nMonsterId, v24) : sub_47900(pGame, pRoom, nMonsterId, v24);
            }
            else
            {
                uint8_t nMinGrp = 1;
                uint8_t nMaxGrp = 1;
                if (!(z.fallenScarab ? z.fallenScarab(nMonsterId) : sub_46D40(nMonsterId)))
                {
                    nMinGrp = pMonStatsTxtRecord->nMinGrp;
                    nMaxGrp = pMonStatsTxtRecord->nMaxGrp;
                }

                if (pMonStatsTxtRecord->nSparsePopulate && (ITEMS_RollRandomNumber(&pGame->pGameSeed) % 100) > pMonStatsTxtRecord->nSparsePopulate)
                {
                    continue;
                }

                bSpawned = z.group ? z.group(pGame, pRoom, pRoomCoordList, nMonsterId, nMinGrp, nMaxGrp) : sub_6FC677D0(pGame, pRoom, pRoomCoordList, nMonsterId, nMinGrp, nMaxGrp) != nullptr;
            }

            if (bSpawned)
            {
                bIncrease = 1;
            }
        }
    }

    if (bIncrease)
    {
        ++pMonsterRegion->unk0x08;
    }
}

//D2Game.0x6FC67570
int32_t __fastcall sub_6FC67570(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, D2RoomCoordListStrc* pRoomCoordList, int32_t nMonsterId, D2UnkMonCreateStrc2* a5, int32_t* pX, int32_t* pY)
{
    int32_t nLeft = 0;
    int32_t nTop = 0;
    int32_t nRight = 0;
    int32_t nBottom = 0;

    if (pRoomCoordList)
    {
        D2DrlgCoordStrc drlgCoord = {};
        memcpy(&drlgCoord, &pRoomCoordList->pBox[1], sizeof(drlgCoord));
        DUNGEON_GameTileToSubtileCoords(&drlgCoord.nPosX, &drlgCoord.nPosY);
        DUNGEON_GameTileToSubtileCoords(&drlgCoord.nWidth, &drlgCoord.nHeight);
        nLeft = drlgCoord.nPosX + 1;
        nRight = drlgCoord.nWidth - (drlgCoord.nPosX + 1);
        nTop = drlgCoord.nPosY + 1;
        nBottom = drlgCoord.nHeight - (drlgCoord.nPosY + 1);
    }
    else
    {
        D2DrlgCoordsStrc drlgCoords = {};
        DUNGEON_GetRoomCoordinates(pRoom, &drlgCoords);
        nLeft = drlgCoords.nSubtileX + 1;
        nTop = drlgCoords.nSubtileY + 1;
        nBottom = drlgCoords.nSubtileHeight - 1;
        nRight = drlgCoords.nSubtileWidth - 1;
    }

    int32_t nRoomCoordListIndex = 0;
    if (pRoomCoordList)
    {
        nRoomCoordListIndex = pRoomCoordList->nIndex;
    }

    D2SeedStrc* pSeed = nullptr;
    if (pRoom)
    {
        pSeed = &pRoom->pSeed;
    }

    for (int32_t i = 0; i < 20; ++i)
    {
        const int32_t nX = ITEMS_RollLimitedRandomNumber(pSeed, nRight) + nLeft;
        const int32_t nY = ITEMS_RollLimitedRandomNumber(pSeed, nBottom) + nTop;

        int32_t j = 0;
        while (j < a5->nRecordCount)
        {
            D2UnitStrc* pMonster = nullptr;
            if (pRoomCoordList)
            {
                if (D2Common_10095(pRoom, nX + a5->records[j].nXOffset, a5->records[j].nYOffset) == nRoomCoordListIndex)
                {
                    pMonster = sub_6FC6A030(pGame, pRoom, pRoomCoordList, nX + a5->records[j].nXOffset, nY + a5->records[j].nYOffset, nMonsterId, 1, -1, 1);
                }
            }
            else
            {
                pMonster = D2GAME_SpawnMonster_6FC69F10(pGame, pRoom, nX + a5->records[j].nXOffset, nY + a5->records[j].nYOffset, nMonsterId, 1, -1, 1);
            }

            if (!pMonster)
            {
                break;
            }

            ++j;
        }

        if (j >= a5->nRecordCount)
        {
            *pX = nX;
            *pY = nY;
            return 1;
        }
    }

    return 0;
}

//D2Game.0x6FC677D0
D2UnitStrc* __fastcall sub_6FC677D0(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, D2RoomCoordListStrc* pRoomCoordList, int32_t nMonsterId, uint8_t nMin, uint8_t nMax)
{
    if (!nMin || !nMax || nMax < nMin)
    {
        return nullptr;
    }

    int32_t nX = 0;
    int32_t nY = 0;
    if (!sub_6FC66260(pGame, pRoom, pRoomCoordList, nMonsterId, &nX, &nY, 1))
    {
        return nullptr;
    }

    D2UnitStrc* pMonster = sub_6FC6A030(pGame, pRoom, pRoomCoordList, nX, nY, nMonsterId, 1, -1, 0);
    if (!pMonster)
    {
        return nullptr;
    }

    --nMin;
    --nMax;

    D2MonStats2Txt* pMonStats2TxtRecord = MONSTERREGION_GetMonStats2TxtRecord(nMonsterId);
    if (pMonStats2TxtRecord && pMonStats2TxtRecord->dwFlags & gdwBitMasks[MONSTATS2FLAGINDEX_OBJCOL] && nMonsterId == MONSTER_EVILHUT)
    {
        SUNIT_AllocUnitData(UNIT_OBJECT, 562, CLIENTS_GetUnitX(pMonster), CLIENTS_GetUnitY(pMonster), pGame, pRoom, 1, 0, 0);
    }

    const int32_t nCount = nMin + ITEMS_RollLimitedRandomNumber(&pMonster->pSeed, nMax - nMin + 1);
    for (int32_t i = 0; i < nCount; ++i)
    {
        sub_6FC69F70(pGame, pRoomCoordList, pMonster, nMonsterId, 1, 3, 0);
    }

    return pMonster;
}

static void sub_47660(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, D2MonsterRegionStrc* pMonsterRegion, D2LevelsTxt* pLevelsTxtRecord);

void sub_47820(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nLevelId, D2LevelsTxt* pLevelsTxtRecord)
{
    if ((int32_t)(ITEMS_RollRandomNumber(&pRoom->pSeed) % 100) >= 3)
    {
        return;
    }

    D2MonsterRegionStrc* pMonsterRegion = MONSTERREGION_GetMonsterRegionFromLevelId(pGame->pMonReg, nLevelId);
    if (pMonsterRegion->unk0x2C4 >= 3)
    {
        return;
    }

    sub_47660(pGame, pRoom, pMonsterRegion, pLevelsTxtRecord);
}

//D2Game.0x6FC679F0
// 1.13c D2Game+0x47820 (thiscall room, stdcall level): roll %100 < 3 + region count gate, then 0x47660
void __fastcall sub_6FC679F0(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom)
{
    if (!pRoom || ITEMS_RollRandomNumber(&pRoom->pSeed) & 0x7FFF || pRoom->nNumClients)
    {
        return;
    }

    const int32_t nLevelId = DUNGEON_GetLevelIdFromRoom(pRoom);
    D2LevelsTxt* pLevelsTxtRecord = DATATBLS_GetLevelsTxtRecord(nLevelId);
    if (!pLevelsTxtRecord || !pLevelsTxtRecord->nMonWndr)
    {
        return;
    }

    sub_47820(pGame, pRoom, nLevelId, pLevelsTxtRecord);
}

// 1.13c D2Game+0x47660 (the part of 1.10f sub_6FC679F0 after the region gate; its caller 0x47820 passes room, game, region)
void (*g_d2Wander47660Seam)(D2GameStrc*, D2ActiveRoomStrc*, D2MonsterRegionStrc*) = nullptr;   // replay seam (kind 3446, s3249), null in the game
// 1.13c D2Game+0x47660
static void sub_47660(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, D2MonsterRegionStrc* pMonsterRegion, D2LevelsTxt* pLevelsTxtRecord)
{
    if (g_d2Wander47660Seam)
    {
        g_d2Wander47660Seam(pGame, pRoom, pMonsterRegion);
        return;
    }

    constexpr int32_t dword_6FD2EE44[] = { MONSTER_ROGUE2 };
    constexpr uint8_t byte_6FD2EE48[10] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

    if (pLevelsTxtRecord->nAct >= 5u)
    {
        return;
    }

    const uint32_t nIndex = 2 * pLevelsTxtRecord->nAct;
    if (!byte_6FD2EE48[nIndex + 1])
    {
        return;
    }

    const int32_t nMonsterId = dword_6FD2EE44[ITEMS_RollLimitedRandomNumber(&pRoom->pSeed, byte_6FD2EE48[nIndex + 1]) + byte_6FD2EE48[nIndex]];
    
    int32_t nX = 0;
    int32_t nY = 0;
    if (!sub_6FC66260(pGame, pRoom, nullptr, nMonsterId, &nX, &nY, 0))
    {
        return;
    }

    D2UnitStrc* pMonster = D2GAME_SpawnMonster_6FC69F10(pGame, pRoom, nX, nY, nMonsterId, 1, -1, 0);
    if (!pMonster)
    {
        return;
    }

    sub_6FC40280(pGame, pMonster, 0, 8);
    ++pMonsterRegion->unk0x2C4;
}

//D2Game.0x6FC67B90
void __fastcall MONSTERREGION_InitializeAll(void* pMemPool, D2MonsterRegionStrc** ppMonsterRegion, D2SeedStrc* pSeed, uint32_t nLowSeed, uint8_t nDifficulty, int32_t bExpansion)
{
    SEED_InitLowSeed(pSeed, nLowSeed);

    ++ppMonsterRegion;

    if (!DATATBLS_GetDifficultyLevelsTxtRecord(nDifficulty))
    {
        FOG_DisplayWarning("pctDifficultyLevel", __FILE__, __LINE__);
    }

    for (int32_t nLevelId = 1; nLevelId < sgptDataTables->nLevelsTxtRecordCount; ++nLevelId)
    {
        D2MonsterRegionStrc* pMonsterRegion = D2_CALLOC_STRC_POOL(pMemPool, D2MonsterRegionStrc);

        D2LevelsTxt* pLevelsTxtRecord = DATATBLS_GetLevelsTxtRecord(nLevelId);
        pMonsterRegion->dwlevel = nLevelId;
        pMonsterRegion->nAct = pLevelsTxtRecord->nAct;
        pMonsterRegion->dwMonDen = pLevelsTxtRecord->dwMonDen[nDifficulty];
        pMonsterRegion->nBossMin = pLevelsTxtRecord->nMonUMin[nDifficulty];
        pMonsterRegion->nBossMax = pLevelsTxtRecord->nMonUMax[nDifficulty];
        pMonsterRegion->nMonWander = pLevelsTxtRecord->nMonWndr;
        pMonsterRegion->unk0x2D4 = -1;
        pMonsterRegion->unk0x0C = -1;
        pMonsterRegion->nQuest = pLevelsTxtRecord->nQuest;

        if (bExpansion)
        {
            pMonsterRegion->dwDungeonLevel = pLevelsTxtRecord->wMonLvlEx[nDifficulty];
            pMonsterRegion->dwDungeonLevelEx = pLevelsTxtRecord->wMonLvlEx[nDifficulty];
        }
        else
        {
            pMonsterRegion->dwDungeonLevel = pLevelsTxtRecord->wMonLvl[nDifficulty];
            pMonsterRegion->dwDungeonLevelEx = pLevelsTxtRecord->wMonLvl[nDifficulty];
        }

        int32_t nNumMonstersToSpawn = std::min(pLevelsTxtRecord->nNumMon, (uint8_t)13);
        int32_t nNumMonstersInSpawnList = pLevelsTxtRecord->nNumNormalMonsters;
        if (nDifficulty > 0)
        {
            nNumMonstersInSpawnList = pLevelsTxtRecord->nNumNightmareHellMonsters;
        }

        nNumMonstersToSpawn = std::min(nNumMonstersToSpawn, nNumMonstersInSpawnList);

        int16_t monsterIds[ARRAY_SIZE(D2LevelsTxt::wNormalMonsters)] = {};
        D2MonRegDataStrc* pMonRegData = pMonsterRegion->pMonData;
        if (nDifficulty > 0)
        {
            if (pLevelsTxtRecord->nNumNightmareHellMonsters > 0)
            {
                memcpy(monsterIds, pLevelsTxtRecord->wNightmareHellMonsters, sizeof(monsterIds[0]) * pLevelsTxtRecord->nNumNightmareHellMonsters);
            }
        }
        else
        {
            if (pLevelsTxtRecord->nNumNormalMonsters > 0)
            {
                memcpy(monsterIds, pLevelsTxtRecord->wNormalMonsters, sizeof(monsterIds[0]) * pLevelsTxtRecord->nNumNormalMonsters);
            }
        }

        for (int32_t i = 0; i < nNumMonstersToSpawn && nNumMonstersInSpawnList > 0; ++i)
        {
            int32_t nIndex = ITEMS_RollLimitedRandomNumber(pSeed, nNumMonstersInSpawnList);
            int32_t nMonsterId = monsterIds[nIndex];
            if (!i && pLevelsTxtRecord->nRangedSpawn)
            {
                for (int32_t j = 0; j < 20; ++j)
                {
                    D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(nMonsterId);
                    if (pMonStatsTxtRecord && pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_RANGEDTYPE])
                    {
                        break;
                    }

                    nIndex = ITEMS_RollLimitedRandomNumber(pSeed, nNumMonstersInSpawnList);
                    nMonsterId = monsterIds[nIndex];
                }
            }
			// Remove monster from list
			const int32_t nPreviousNumMonstersInSpawnList = nNumMonstersInSpawnList;
            --nNumMonstersInSpawnList;
            if (nPreviousNumMonstersInSpawnList > (nIndex+1))
            {
				// Note: original game does a memcpy here, but we have overlapping inputs.
                memmove(&monsterIds[nIndex], &monsterIds[nIndex + 1], sizeof(monsterIds[0]) * (nNumMonstersInSpawnList - nIndex));
            }

            D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(nMonsterId);
            if (pMonStatsTxtRecord && (pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_ISSPAWN]))
            {
                pMonRegData->nMonHcIdx = nMonsterId;
                pMonRegData->nRarity = pMonStatsTxtRecord->nRarity;
                pMonsterRegion->nTotalRarity += pMonStatsTxtRecord->nRarity;
                ++pMonsterRegion->nSpawnCount;
                ++pMonsterRegion->nMonCount;
                ++pMonRegData;
            }
        }

        for (int32_t i = 0; i < pMonsterRegion->nMonCount; ++i)
        {
            sub_6FC62020(pSeed, &pMonsterRegion->pMonData[i], 3);
        }

        MONSTERREGION_StorePackedPointer(ppMonsterRegion, pMonsterRegion);
        ++ppMonsterRegion;
    }
}

//D2Game.0x6FC67F30
void __fastcall MONSTERREGION_FreeAll(void* pMemPool, D2MonsterRegionStrc** ppMonsterRegion)
{
    for (int32_t i = 1; i < sgptDataTables->nLevelsTxtRecordCount; ++i)
    {        
        D2MonsterRegionStrc* pMonsterRegion = MONSTERREGION_LoadPackedPointer(ppMonsterRegion + i);
        if (pMonsterRegion)
        {
            D2_FREE_POOL(pMemPool, pMonsterRegion);
        }
    }
}

//D2Game.0x6FC67F90
D2MonsterRegionStrc* __fastcall MONSTERREGION_GetMonsterRegionFromLevelId(D2MonsterRegionStrc** ppMonsterRegion, int32_t nLevelId)
{
    return MONSTERREGION_LoadPackedPointer(ppMonsterRegion + nLevelId);
}

//D2Game.0x6FC67FA0
// 1.13c D2Game+0xCA5F0
D2MonRegDataStrc* __fastcall sub_6FC67FA0(D2MonsterRegionStrc** ppMonsterRegion, D2ActiveRoomStrc* pRoom, D2UnitStrc* pUnit)
{
    int32_t nClassId = -1;
    if (pUnit)
    {
        nClassId = pUnit->dwClassId;
    }

    switch (MONSTERREGION_SanitizeMonsterId(nClassId))
    {
    case MONSTER_ACT2MALE:
    case MONSTER_ACT2FEMALE:
    case MONSTER_ACT3MALE:
    case MONSTER_ACT3FEMALE:
        return nullptr;
	default:
		break;
    }

    D2MonsterRegionStrc* pMonsterRegion = MONSTERREGION_LoadPackedPointer(ppMonsterRegion + DUNGEON_GetLevelIdFromRoom(pRoom));
    if (!pMonsterRegion)
    {
        return nullptr;
    }

    D2MonRegDataStrc* pMonsterRegionData = pMonsterRegion->pMonData;
    const int32_t nSpawnCount = pMonsterRegion->nSpawnCount;
    int32_t nCounter = 0;
    while (nCounter < nSpawnCount)
    {
        if (pMonsterRegionData->nMonHcIdx == nClassId)
        {
            return pMonsterRegionData;
        }

        ++pMonsterRegionData;
        ++nCounter;
    }

    if (nCounter >= 13)
    {
        return nullptr;
    }

    D2MonStats2Txt* pMonStats2TxtRecord = MONSTERREGION_GetMonStats2TxtRecord(nClassId);
    if (pMonStats2TxtRecord && pMonStats2TxtRecord->nTotalPieces > 2u)
    {
        pMonsterRegionData->nMonHcIdx = nClassId;
        ++pMonsterRegion->nSpawnCount;
        sub_6FC62020(&pUnit->pSeed, pMonsterRegionData, 3);
    }

    return pMonsterRegionData;
}

//D2Game.0x6FC68110
// 1.13c D2Game+0xCACC0
void __fastcall sub_6FC68110(D2GameStrc* pGame)
{
    D2SeedStrc seed = {};
    SEED_InitSeed(&seed);

    const int32_t nSeed = ITEMS_RollRandomNumber(&pGame->pGameSeed);
    SEED_InitLowSeed(&seed, nSeed);
    pGame->dwMonSeed = nSeed;
    MONSTERREGION_InitializeAll(pGame->pMemoryPool, pGame->pMonReg, &seed, nSeed, pGame->nDifficulty, pGame->bExpansion);
}

//D2Game.0x6FC68180
void __fastcall sub_6FC68180(D2MonsterRegionStrc** ppMonsterRegion, D2ActiveRoomStrc* pRoom, D2UnitStrc* pUnit, int32_t bToggleFlag)
{
    if (bToggleFlag)
    {
        MONSTER_ToggleSummonerFlag(pUnit, 2, 1);
    }
    else
    {
        D2MonsterRegionStrc* pMonsterRegion = MONSTERREGION_LoadPackedPointer(ppMonsterRegion + DUNGEON_GetLevelIdFromRoom(pRoom));
        ++pMonsterRegion->dwMonSpawnCount;
    }
}

//D2Game.0x6FC681C0
void __fastcall sub_6FC681C0(D2MonsterRegionStrc** ppMonsterRegion, D2UnitStrc* pUnit, int32_t nPreviousAlignment, int32_t nNewAlignment)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && !SUNIT_IsDead(pUnit) && !MONSTER_CheckSummonerFlag(pUnit, 2u))
    {
        D2MonsterRegionStrc* pMonsterRegion = MONSTERREGION_LoadPackedPointer(ppMonsterRegion + MONSTER_GetLevelId(pUnit));
        if (pMonsterRegion && !pMonsterRegion->nQuest && nPreviousAlignment != nNewAlignment)
        {
            if (nPreviousAlignment == UNIT_ALIGNMENT_EVIL && (nNewAlignment == UNIT_ALIGNMENT_NEUTRAL || nNewAlignment == UNIT_ALIGNMENT_GOOD))
            {
                --pMonsterRegion->dwMonSpawnCount;
            }

            if (nNewAlignment == UNIT_ALIGNMENT_EVIL && nPreviousAlignment != UNIT_ALIGNMENT_UNASSIGNED)
            {
                ++pMonsterRegion->dwMonSpawnCount;
            }
        }
    }
}

//D2Game.0x6FC68240
void __fastcall sub_6FC68240(D2MonsterRegionStrc** ppMonsterRegion, D2UnitStrc* pUnit)
{
    if (!MONSTER_CheckSummonerFlag(pUnit, 2u))
    {
        D2MonsterRegionStrc* pMonsterRegion = MONSTERREGION_LoadPackedPointer(ppMonsterRegion + MONSTER_GetLevelId(pUnit));
        if (STATLIST_GetUnitAlignment(pUnit) == UNIT_ALIGNMENT_EVIL)
        {
            ++pMonsterRegion->dwMonKillCount;
        }
    }
}

//D2Game.0x6FC68280
void __fastcall sub_6FC68280(D2MonsterRegionStrc** ppMonsterRegion, D2UnitStrc* pUnit)
{
    if (!MONSTER_CheckSummonerFlag(pUnit, 2u))
    {
        D2MonsterRegionStrc* pMonsterRegion = MONSTERREGION_LoadPackedPointer(ppMonsterRegion + MONSTER_GetLevelId(pUnit));
        if (!STATLIST_GetUnitAlignment(pUnit))
        {
            --pMonsterRegion->dwMonKillCount;
        }
    }
}

//D2Game.0x6FC682C0
void __fastcall sub_6FC682C0(D2MonsterRegionStrc** ppMonRegion, int32_t nLevelId1, int32_t nLevelId2, int32_t nAlignment, int32_t bDead, int32_t a6, D2UnitStrc* pUnit)
{
    D2MonsterRegionStrc* pMonsterRegion1 = MONSTERREGION_LoadPackedPointer(ppMonRegion + nLevelId1);
    D2MonsterRegionStrc* pMonsterRegion2 = MONSTERREGION_LoadPackedPointer(ppMonRegion + nLevelId2);
    if (pUnit)
    {
        if (pMonsterRegion2 != pMonsterRegion1)
        {
            MONSTER_SetLevelId(pUnit, nLevelId1);
        }

        if (!a6 && STATLIST_GetUnitAlignment(pUnit) == UNIT_ALIGNMENT_EVIL)
        {
            --pMonsterRegion2->dwMonSpawnCount;

            if (MONSTERUNIQUE_CheckMonTypeFlag(pUnit, MONTYPEFLAG_OTHER))
            {
                --pMonsterRegion2->dwUniqueCount;
            }
        }
    }
    else
    {
        if (!a6 && !bDead && nAlignment == UNIT_ALIGNMENT_EVIL)
        {
            --pMonsterRegion1->dwMonSpawnCount;
        }
    }
}
