#include "UNIT/SUnit.h"
#include <test_seam.h>
#include <cstdio>
#include <test_seam.h>

#include <D2BitManip.h>

#include <D2Dungeon.h>
#include <Drlg/D2DrlgDrlg.h>
#include <Drlg/D2DrlgPreset.h>
#include <Units/UnitRoom.h>
#include <D2StatList.h>
#include <D2Collision.h>
#include <D2States.h>
#include <D2Monsters.h>
#include <D2Items.h>
#include <DataTbls/MissilesTbls.h>   // the missile's own sequence record (SUNIT_AllocUnitData, UNIT_MISSILE)
#include <DataTbls/MonsterIds.h>
#include <DataTbls/ObjectsIds.h>
#include <DataTbls/LevelsIds.h>
#include <Path/PathMisc.h>

#include <D2Inventory.h>
#include <cstdlib>


#include "AI/AiGeneral.h"
#include "GAME/Arena.h"
#include "GAME/Clients.h"
#include "GAME/Event.h"
#include "GAME/Game.h"
#include "GAME/SCmd.h"
#include "GAME/Targets.h"
#include "INVENTORY/InvMode.h"
#include "ITEMS/ItemMode.h"
#include "ITEMS/Items.h"

// not-1.13c: d2-web helper for a pointer slot of a packed struct (retail reads the slot with a plain mov); no game rule
template <class T>
static T* SUNIT_LoadPackedPointer(T* const* pSlot)
{
    T* value = nullptr;
    volatile const uint8_t* src = reinterpret_cast<volatile const uint8_t*>(pSlot);
    uint8_t* dst = reinterpret_cast<uint8_t*>(&value);
    for (size_t i = 0; i < sizeof(value); ++i) dst[i] = src[i];
    return value;
}

// not-1.13c: d2-web helper for a pointer slot of a packed struct (retail writes the slot with a plain mov); no game rule
template <class T>
static void SUNIT_StorePackedPointer(T** pSlot, T* value)
{
    const uint8_t* src = reinterpret_cast<const uint8_t*>(&value);
    volatile uint8_t* dst = reinterpret_cast<volatile uint8_t*>(pSlot);
    for (size_t i = 0; i < sizeof(value); ++i) dst[i] = src[i];
}
#include "MISSILES/Missiles.h"
#include "MONSTER/Monster.h"
#include "MONSTER/MonsterAI.h"
#include "MONSTER/MonsterMode.h"
#include "MONSTER/MonsterRegion.h"
#include "OBJECTS/Objects.h"
#include "OBJECTS/ObjMode.h"
#include "PLAYER/Player.h"
#include "PLAYER/PlayerList.h"
#include "PLAYER/PlayerPets.h"
#include "PLAYER/PlrModes.h"
#include "QUESTS/Quests.h"
#include "SKILLS/SkillAss.h"
#include "UNIT/Party.h"
#include "UNIT/SUnitDmg.h"
#include "UNIT/SUnitEvent.h"
#include "UNIT/SUnitMsg.h"
#include "UNIT/SUnitProxy.h"
#include "core/d2game/fn_rules.h"  // rules shared with the FN probes


#pragma warning(disable: 28159)


extern uint32_t dword_6FD4DC40;   // d2-web: defined in PLAYER/PlayerList.cpp (upstream defines it twice)



// 1.13c D2Game+0xE0BF0
// The player's interact reset: looks up the unit the player was interacting with, lets go of it, and clears the interact slot.
// Returns retail's EAX (nothing reads it): the interact type when nothing was interacting, 0 when the unit is gone, a callee's result
// where the retail callee returns one (SUNIT_EAX_FROM_CALLEE, the FN probe tracks it), else the object timer pointer or type - 2.
int64_t __fastcall SUNIT_ResetPlayerInteract(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    if (!(uint8_t)pUnit->nInteract)
    {
        return pUnit->dwInteractType;
    }

    D2UnitStrc* pInteractUnit = D2_CALL(SUNIT_GetServerUnit, pGame, pUnit->dwInteractType, pUnit->dwInteractGUID);
    if (!pInteractUnit)
    {
        return 0;
    }

    int64_t nResult = SUNIT_EAX_FROM_CALLEE;
    if (pInteractUnit->dwUnitType == UNIT_MONSTER)
    {
        D2_CALL(MONSTERAI_RemoveInteractInfoFor, pGame, pUnit, pInteractUnit->pMonsterData ? pInteractUnit->pMonsterData->pMonInteract : nullptr);
        D2_CALL(SUNITPROXY_FreeVendorChain, pGame, pUnit, pInteractUnit);
    }
    else
    {
        nResult = (int64_t)(uint32_t)(pInteractUnit->dwUnitType - UNIT_OBJECT);
        if (pInteractUnit->dwUnitType == UNIT_OBJECT)
        {
            if (pInteractUnit->dwClassId == OBJECT_STEEG_STONE)
            {
                D2TimerArg2Strc* pArg = D2_CALL(UNITS_GetTimerArg, pInteractUnit)->unk0x00;
                if (pArg)
                {
                    pArg->nUnitGUID = -1;
                }
                nResult = (int64_t)(uintptr_t)pArg;
            }
            else
            {
                D2_CALL(UNITS_ChangeAnimMode, pInteractUnit, 0);
                nResult = SUNIT_EAX_FROM_CALLEE;
            }
        }
    }

    pUnit->dwInteractGUID = -1;
    pUnit->dwInteractType = 6;
    pUnit->nInteract &= 0xFF00;
    return nResult;
}

// 1.13c D2Game+0xE02A0
// Unlinks the unit from its game unit list (hash bucket by unit id, or the tile list).
void __fastcall SUNIT_UnlinkFromGameList(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    D2UnitStrc** ppUnitList = nullptr;
    if (pUnit->dwUnitType == UNIT_TILE)
    {
        ppUnitList = &pGame->pTileList;
    }
    else
    {
        ppUnitList = SUNIT_GetUnitList(pUnit->dwUnitType, pGame, pUnit->dwUnitId);
    }

    // pUnitList lives in the packed retail D2GameStrc. Its wasm64 pointer
    // slots are not necessarily eight-byte aligned, so load/store via memcpy.
    D2UnitStrc* pListHead = nullptr;
    pListHead = SUNIT_LoadPackedPointer(ppUnitList);
    D2UnitStrc* pPrevious = nullptr;
    for (D2UnitStrc* i = pListHead; i; i = i->pListNext)
    {
        if (i == pUnit)
        {
            if (pPrevious)
            {
                pPrevious->pListNext = i->pListNext;
            }
            else
            {
                pListHead = i->pListNext;
                SUNIT_StorePackedPointer(ppUnitList, pListHead);
            }
            break;
        }
        pPrevious = i;
    }
}

// 1.13c D2Game+0xE0C80
// The first half of unit removal: the player's interact reset, the common unit detach (kinds 0..3), the room removal (unless nKeepRoom), the game list unlink.
void __fastcall SUNIT_DetachUnit(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nKeepRoom)
{
    D2_ASSERT(pUnit);
    D2_ASSERT(pUnit->dwUnitType < 6);

    if (pUnit->dwUnitType == UNIT_PLAYER)
    {
        D2_CALL(SUNIT_ResetPlayerInteract, pGame, pUnit);
    }

    if (pUnit->dwUnitType <= UNIT_MISSILE)
    {
        D2_CALL(D2Common_10228, pUnit);
    }

    if (!nKeepRoom)
    {
        D2_CALL(UNITROOM_RemoveUnitFromRoom, pUnit);
    }

    D2_CALL(SUNIT_UnlinkFromGameList, pGame, pUnit);
}

//D2Game.0x6FCBAEE0
// 1.13c D2Game+0xE20A0 (checked against decompile, no delta; first half is retail 0xE0C80)
void __fastcall SUNIT_RemoveUnit(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    D2_ASSERT(pGame);
    D2_ASSERT(pUnit);
    D2_ASSERT(pUnit->dwUnitType < 6);

    SUNIT_DetachUnit(pGame, pUnit, 0);

    if (pUnit->pQuestEventList)
    {
        QUESTS_FreeChainRecord(pGame, pUnit->pQuestEventList);
        pUnit->pQuestEventList = nullptr;
    }

    switch (pUnit->dwUnitType)
    {
    case UNIT_PLAYER:
    {
        D2GAME_FreeUnitNode_6FC40310(pGame, pUnit);
        sub_6FC7C170(pGame, pUnit);
        PARTY_LeaveParty(pGame, pUnit);
        PLAYER_Destroy(pGame, pUnit);

        if (pUnit->pDynamicPath)
        {
            PATH_FreeDynamicPath(pGame->pMemoryPool, pUnit->pDynamicPath);
            pUnit->pDynamicPath = nullptr;
        }
        break;
    }
    case UNIT_MONSTER:
    {
        D2GAME_TARGETS_Last_6FC40380(pGame, pUnit);

        if (pUnit->pDynamicPath)
        {
            PATH_FreeDynamicPath(pGame->pMemoryPool, pUnit->pDynamicPath);
            pUnit->pDynamicPath = nullptr;
        }

        MONSTER_Free(pGame, pUnit);
        break;
    }
    case UNIT_OBJECT:
    {
        OBJECTS_FreeHoverMessage(pGame, pUnit);
        UNITS_FreeStaticPath(pUnit);
        D2_FREE_POOL(pGame->pMemoryPool, pUnit->pPlayerData);
        break;
    }
    case UNIT_MISSILE:
    {
        if (pUnit->pDynamicPath)
        {
            PATH_FreeDynamicPath(pGame->pMemoryPool, pUnit->pDynamicPath);
            pUnit->pDynamicPath = nullptr;
        }

        MISSILES_Free(pGame, pUnit);
        break;
    }
    case UNIT_ITEM:
    {
        sub_6FC502B0(pGame, pUnit);
        ITEMS_FreeItemData(pGame->pMemoryPool, pUnit);
        UNITS_FreeStaticPath(pUnit);
        break;
    }
    case UNIT_TILE:
    {
        UNITS_FreeStaticPath(pUnit);
        break;
    }
    default:
        break;
    }

    SUNITEVENT_FreeEventList(pGame, pUnit);
    D2GAME_SUNITMSG_FreeUnitMessages_6FCC6790(pUnit);
    UNITS_FreeUnit(pUnit);
}

//D2Game.0x6FCBB160
// not-1.13c: retail has no function for this; the list walks read unit+0xE4 in place (e.g. D2Game+0xE0310, +0xE03A0)
D2UnitStrc* __fastcall SUNIT_GetNextUnitFromList(D2UnitStrc* pUnit)
{
    D2_ASSERT(pUnit);

    return pUnit->pListNext;
}

// 1.13c D2Game+0xDF620
// The unit's queued next room, if it is still in the room list of the unit's act; otherwise the next-room reference is cleared and 0 returned.
D2ActiveRoomStrc* __fastcall SUNIT_FindNextRoomInAct(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    D2DynamicPathStrc* pPath = pUnit->pDynamicPath;
    D2ActiveRoomStrc* pNextRoom = D2_CALL(PATH_GetNextRoom, pPath);
    const int32_t nLevelId = D2_CALL(DUNGEON_GetLevelIdFromRoom, D2_CALL(UNITS_GetRoom, pUnit));
    if (nLevelId)
    {
        const uint8_t nActNo = D2_CALL(DRLG_GetActNoFromLevelId, nLevelId);
        for (D2ActiveRoomStrc* pTemp = D2_CALL(DUNGEON_GetRoomFromAct, pGame->pAct[nActNo]); pTemp; pTemp = pTemp->pRoomNext)
        {
            if (pTemp == pNextRoom)
            {
                return pNextRoom;
            }
        }
    }

    D2_CALL(PATH_ClearNextRoom, pPath);
    return nullptr;
}

//D2Game.0x6FCBB190
// 1.13c D2Game+0xE1610
void __fastcall sub_6FCBB190(D2GameStrc* pGame, D2UnitStrc* pUnit, D2ActiveRoomStrc* pRoom)
{
    if (!UNITS_IsCurrentRoomInvalid(pUnit))
    {
        return;
    }

    D2ActiveRoomStrc* pCurrentRoom = UNITS_GetRoom(pUnit);
    D2ActiveRoomStrc* pPreviousRoom = pRoom;
    if (pRoom)
    {
        if (pCurrentRoom == pPreviousRoom)
        {
            return;
        }
    }
    else
    {
        pPreviousRoom = SUNIT_FindNextRoomInAct(pGame, pUnit);
        if (pPreviousRoom && pCurrentRoom == pPreviousRoom)
        {
            return;
        }
    }

    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
    {
        pUnit->pMonsterData->pVision = 0;
    }

    D2ClientStrc** ppClientsCurr = nullptr;
    int32_t nCurrClients = 0;
    if (pCurrentRoom)
    {
        ppClientsCurr = pCurrentRoom->ppClients;
        nCurrClients = pCurrentRoom->nNumClients;
    }

    D2ClientStrc** ppClientsPrev = nullptr;
    int32_t nPrevClients = 0;
    if (pPreviousRoom)
    {
        ppClientsPrev = pPreviousRoom->ppClients;
        nPrevClients = pPreviousRoom->nNumClients;
    }

    int32_t nCurrentCounter = 0;
    int32_t nPreviousCounter = 0;
    if (nCurrClients)
    {
        while (nPreviousCounter < nPrevClients && nCurrentCounter < nCurrClients)
        {
            D2_ASSERT(ppClientsCurr);
            D2_ASSERT(ppClientsPrev);

            D2ClientStrc* pCurrentClient = ppClientsCurr[nCurrentCounter];
            D2ClientStrc* pPreviousClient = ppClientsPrev[nPreviousCounter];
            if (pCurrentClient >= pPreviousClient)
            {
                if (pCurrentClient <= pPreviousClient)
                {
                    ++nCurrentCounter;
                }
                else
                {
                    if (pUnit != CLIENTS_GetPlayerFromClient(pPreviousClient, 0))
                    {
                        D2GAME_PACKETS_SendPacket0x0A_RemoveObject_6FCC5F80(pUnit, ppClientsPrev[nPreviousCounter]);
                    }
                }

                ++nPreviousCounter;
            }
            else
            {
                if (pUnit != CLIENTS_GetPlayerFromClient(pCurrentClient, 0))
                {
                    D2GAME_SUNITMSG_FirstFn_6FCC5520(pGame, pUnit, ppClientsCurr[nCurrentCounter]);
                }

                ++nCurrentCounter;
            }
        }

        while (nCurrentCounter < nCurrClients)
        {
            D2_ASSERT(ppClientsCurr);

            if (pUnit != CLIENTS_GetPlayerFromClient(ppClientsCurr[nCurrentCounter], 0))
            {
                D2GAME_SUNITMSG_FirstFn_6FCC5520(pGame, pUnit, ppClientsCurr[nCurrentCounter]);
            }

            ++nCurrentCounter;
        }
    }

    while (nPreviousCounter < nPrevClients)
    {
        D2_ASSERT(ppClientsPrev);

        if (pUnit != CLIENTS_GetPlayerFromClient(ppClientsPrev[nPreviousCounter], 0))
        {
            D2GAME_PACKETS_SendPacket0x0A_RemoveObject_6FCC5F80(pUnit, ppClientsPrev[nPreviousCounter]);
        }

        ++nPreviousCounter;
    }

    PATH_SetCurrentRoomInvalid(pUnit->pDynamicPath, 0);
}

//D2Game.0x6FCBB440
// 1.13c D2Game+0xE1C00
void __fastcall SUNIT_WarpPlayer(D2GameStrc* pGame, D2UnitStrc* pTarget, D2UnitStrc* pRoomTile)
{
    D2_ASSERT(pGame);
    D2_ASSERT(pTarget);

    D2ActiveRoomStrc* pTilesRoom = D2_CALL(UNITS_GetRoom, pRoomTile);

    D2LvlWarpTxt* pLvlWarpTxtRecord = nullptr;
    D2UnitStrc* pWarpTile = DUNGEON_GetWarpTileFromRoomAndSourceLevelId(pTilesRoom, pRoomTile ? pRoomTile->dwClassId : -1, &pLvlWarpTxtRecord);
    if (!pWarpTile)
    {
        return;
    }

    D2CoordStrc pCoord = {};
    D2ActiveRoomStrc* pWarpTileRoom = D2_CALL(UNITS_GetRoom, pWarpTile);
    pCoord.nX = CLIENTS_GetUnitX(pWarpTile);
    pCoord.nY = CLIENTS_GetUnitY(pWarpTile);

    D2_ASSERT(pTarget->dwUnitType == UNIT_PLAYER);

    D2ActiveRoomStrc* pDestinationRoom = COLLISION_GetFreeCoordinates(pWarpTileRoom, &pCoord, UNITS_GetUnitSizeX(pTarget), 0x1C09u, 1);
    if (!pDestinationRoom)
    {
        return;
    }

    const int32_t nSourceLevel = DUNGEON_GetLevelIdFromRoom(pTilesRoom);
    const int32_t nDestinationLevel = DUNGEON_GetLevelIdFromRoom(pDestinationRoom);
    if ((nDestinationLevel == LEVEL_DURIELSLAIR || nDestinationLevel == LEVEL_THEWORLDSTONEKEEPLEV1 || nDestinationLevel == LEVEL_THEWORLDSTONECHAMBER || nDestinationLevel == LEVEL_GLACIALCAVESLEV1 || nDestinationLevel == LEVEL_DURANCEOFHATELEV1)
        && QUESTS_LevelWarpCheck(pGame, pTarget, nSourceLevel, nDestinationLevel))
    {
        return;
    }

    int32_t nX = pCoord.nX;
    int32_t nY = pCoord.nY;
    // 1.13c D2Game+0xE19C0
    if (!D2_CALL(sub_6FCBDFE0, pGame, pTarget, pWarpTileRoom, nX, nY, 0, 0))
    {
        return;
    }

    nX += pLvlWarpTxtRecord->dwExitWalkX;
    nY += pLvlWarpTxtRecord->dwExitWalkY;
    D2_CALL(sub_6FC817D0, pGame, pTarget, 0, 2, nX, nY, 0);
    D2GAME_PACKETS_SendPacket0x0D_6FC3C920(D2_CALL(UNITS_GetPlayerData, pTarget)->pClient, 0xDu, pTarget->dwUnitType, pTarget->dwUnitId, 1, nX, nY, 0, UNITS_GetCurrentLifePercentage(pTarget));
}

// 1.13c D2Game+0xE0110 (checked against decompile and bytes, no delta)
void __fastcall SUNIT_InitSeed(D2UnitStrc* pUnit, D2SeedStrc* pSeed)
{
    D2_ASSERT(pUnit);

    SEED_InitSeed(&pUnit->pSeed);

#ifdef D2_MONS_TRACE
    if (pSeed && d2_trace_level >= 0) fprintf(stderr, "unitseed type %d cls %d game %u/%u\n", pUnit->dwUnitType, pUnit->dwClassId, pSeed->nLowSeed, pSeed->nHighSeed);
#endif
    if (pSeed)
    {
        pUnit->dwInitSeed = ITEMS_RollRandomNumber(pSeed);
    }
    else
    {
        pUnit->dwInitSeed = SEED_GetRandomValue(2 * dword_6FD4DC40);
        ++dword_6FD4DC40;
    }

    SEED_InitLowSeed(&pUnit->pSeed, pUnit->dwInitSeed);
}

// 1.13c D2Game+0xE00C0: give a freshly allocated item its seed pair. The item seed starts at {1, 666}, one game-seed roll is taken and
// stored as the item's start seed, and the item seed's low word becomes that start seed (high stays 666). Returns the item seed
// pointer, which is what retail leaves in EAX.
D2SeedStrc* SUNIT_InitItemSeeds(D2UnitStrc* pUnit, D2SeedStrc* pGameSeed)
{
    ITEMS_InitItemSeed(pUnit);
    ITEMS_SetItemStartSeed(pUnit, static_cast<int32_t>(ITEMS_RollRandomNumber(pGameSeed)));

    const int32_t nStartSeed = ITEMS_GetItemStartSeed(pUnit);
    D2SeedStrc* pItemSeed = ITEMS_GetItemSeed(pUnit);
    SEED_InitLowSeed(pItemSeed, nStartSeed);
    return pItemSeed;
}

// 1.13c D2Game+0x8F470: EAX inventory, stack game. Removes every item unit of the inventory; the next node is read before the removal
// (deletion invalidates the node and its item data). Retail reports Fog assertion 0x1c6 and exits on a node that is not an item.
void D2GAME_DrainInventory_113c(D2InventoryStrc* pInventory, D2GameStrc* pGame)
{
    if (!pInventory)
    {
        return;
    }

    for (D2UnitStrc* pNode = INVENTORY_GetFirstItem(pInventory); pNode;)
    {
        D2UnitStrc* pItem = INVENTORY_UnitIsItem(pNode);
        D2UnitStrc* pNext = INVENTORY_GetNextItem(pNode);
        if (!pItem)
        {
            std::exit(-1);
        }
        (void)INVENTORY_GetItemNodePage(pNode); // #10698 is a getter, not a removal
        SUNIT_RemoveUnit(pGame, pItem);
        pNode = pNext;
    }
}

//D2Game.0x6FCBB6C0
// 1.13c D2Game+0xE1D90 (type in ECX, class in EDX; the GUID counter is D2Game+0xDF9C0, inlined here with the same
// wrap and type<6 fatal; the item case passes no room to +0x10160; the missile case only sets the mode)
D2UnitStrc* __fastcall SUNIT_AllocUnitData(int32_t nUnitType, int32_t nClassId, int32_t nX, int32_t nY, D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, char a7, int32_t nMode, DWORD a3)
{
    if (nUnitType == UNIT_PLAYER)
    {
        if ((uint32_t)nClassId >= 7)
        {
            return nullptr;
        }
    }
    else if (nUnitType == UNIT_MONSTER)
    {
        D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(nClassId);
        if (!pMonStatsTxtRecord || !(pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_ENABLED]))
        {
            return nullptr;
        }
    }

    D2UnitStrc* pUnit = UNITS_AllocUnit(pGame->pMemoryPool, nUnitType);
    if (!pUnit)
    {
        return nullptr;
    }

    pUnit->dwUnitType = nUnitType;
    pUnit->dwClassId = nClassId;
    pUnit->pGame = pGame;
    pUnit->pMemoryPool = pGame->pMemoryPool;

    if (pGame->bExpansion)
    {
        pUnit->dwFlagEx |= UNITFLAGEX_ISEXPANSION;
    }
    else
    {
        pUnit->dwFlagEx &= ~UNITFLAGEX_ISEXPANSION;
    }
    pUnit->dwFlagEx |= UNITFLAGEX_SERVERUNIT;

    pUnit->nAct = DRLG_GetActNoFromLevelId(DUNGEON_GetLevelIdFromRoom(pRoom));
    pUnit->pDrlgAct = pGame->pAct[pUnit->nAct];
    pUnit->pQuestEventList = nullptr;

    if (nUnitType)
    {
        SUNIT_InitSeed(pUnit, pGame ? &pGame->pGameSeed : nullptr);

        if (nUnitType == UNIT_ITEM)
        {
            ITEMS_AllocItemData(pGame->pMemoryPool, pUnit);
            SUNIT_InitItemSeeds(pUnit, &pGame->pGameSeed);
        }
    }

    pUnit->dwNodeIndex = 11;
    pUnit->dwFlags |= UNITFLAG_INITSEEDSET;

    uint32_t nUnitGUID = 0;
    if (nUnitType == UNIT_MONSTER && (a7 & 2) != 0)
    {
        nUnitGUID = a3;
    }
    else
    {
        D2_ASSERT(nUnitType < 6);

        nUnitGUID = pGame->dwLastUsedUnitGUID[nUnitType] + 1;
        if (pGame->dwLastUsedUnitGUID[nUnitType] == -2)
        {
            nUnitGUID = 1;
        }

        pGame->dwLastUsedUnitGUID[nUnitType] = nUnitGUID;
    }

    switch (nUnitType)
    {
    case UNIT_PLAYER:
    {
        PLAYER_Create(pGame, pUnit, nUnitGUID);

        if (nMode != PLRMODE_DEAD)
        {
            D2GAME_TARGETS_First_6FC40170(pGame, pUnit, 0);
        }
        break;
    }
    case UNIT_MONSTER:
    {
        pUnit->dwAnimMode = nMode;
        MONSTER_Initialize(pGame, pRoom, pUnit, nUnitGUID);
        break;
    }
    case UNIT_OBJECT:
    {
        pUnit->pObjectData = D2_CALLOC_STRC_POOL(pGame->pMemoryPool, D2ObjectDataStrc);
        pUnit->pObjectData->pObjectTxt = DATATBLS_GetObjectsTxtRecord(nClassId);
        pUnit->dwAnimMode = nMode;
        // The object's sequence record -- its own Objects.txt FrameCnt/FrameDelta/Start for the mode
        // it is in -- is derived here, the same three fields `UNITS_SetAnimStartFrame` (#10349) writes
        // on a mode change. Without it an object that spawns in a mode nothing ever changes (a town
        // bonfire `RB`, a dummy `N2`, a bank `b6`) carries `dwFrameCountPrecise` 0 and `wAnimSpeed` 0,
        // so no per-frame pass can move its clock at all. Retail's own four consecutive town frames
        // advance exactly those objects at exactly their own FrameDelta (`RB` 7,8,8,9 and `N2`
        // 2,3,3,4 at 128, `b6` 0,0,0,0 at 256 with a one-frame sequence;
        // the retail captures) and hold the one whose FrameDelta is 0 (`ss`), which is what
        // `s351-object-animation` measures against.
        //
        // Deliberately NOT the whole of #10349: an object whose row has Sync 0 gets a jittered
        // `wAnimSpeed` there, from `SEED_RollLimitedRandomNumber(&pUnit->pSeed, wFrameDelta / 8)`. That
        // would consume a draw from the unit's own spawn seed, which other code then uses -- the
        // desert shrine's id is the NEXT roll in `OBJECTS_SpawnShrine`, and a shrine's operate function
        // rolls its own seed for its item count (`ObjMode.cpp` 1999/2072) -- so taking it here would
        // move game state this band has no oracle for. The rate it gives up is at most
        // `FrameDelta / 16 + FrameDelta / 8` of 256, a phase difference inside the band `s351` checks.
        // What would settle it: a capture of an object's drawn rate at spawn against its FrameDelta.
        // D19: the server unit keeps retail's own clock (zero until a mode change); the client pass runs a shadow clock (g_clientObjectClock).
        UNITS_AllocStaticPath(pUnit);
        OBJECTS_InitHandler(pGame, pUnit, nUnitGUID, pRoom, nX, nY);
        break;
    }
    case UNIT_ITEM:
    {
        pUnit->dwAnimMode = nMode;
        UNITS_AllocStaticPath(pUnit);
        sub_6FC4BE80(pUnit, pGame, nUnitGUID, pRoom);
        break;
    }
    case UNIT_MISSILE:
    {
        pUnit->dwAnimMode = nMode;
        MISSILES_Initialize(pGame, pUnit, nUnitGUID);
        break;
    }
    case UNIT_TILE:
    {
        UNITS_AllocStaticPath(pUnit);
        pUnit->dwUnitId = nUnitGUID;
        pUnit->dwFlags |= UNITFLAG_TARGETABLE;
        break;
    }
    default:
        break;
    }

    if (a7 & 1)
    {
        SUNIT_Add(pUnit, nX, nY, pGame, pRoom, 1);
        { extern void d2client_unitseed_created(D2UnitStrc*); d2client_unitseed_created(pUnit); }   // d2-web: the client builds it from its unit-add packet

        if (nUnitType == UNIT_PLAYER)
        {
            if (pUnit->dwAnimMode == PLRMODE_DEAD || pUnit->dwAnimMode == PLRMODE_DEATH)
            {
                PATH_RemoveCollisionFootprintForUnit(pUnit, 1);
                PATH_SetUnitCollisionPattern(pUnit, COLLISION_PATTERN_SMALL_NO_PRESENCE);
                PATH_SetFootprintCollisionMask(pUnit->pDynamicPath, COLLIDE_CORPSE);
            }
        }
        else if (nUnitType == UNIT_MONSTER)
        {
            if (MONSTERS_IsDead(pUnit))
            {
                D2MonStats2Txt* pMonStatsTxt2Record = MONSTERREGION_GetMonStats2TxtRecord(nClassId);
                if (!pMonStatsTxt2Record || !(pMonStatsTxt2Record->dwFlags & gdwBitMasks[MONSTATS2FLAGINDEX_DEADCOL]))
                {
                    PATH_RemoveCollisionFootprintForUnit(pUnit, 1);
                    PATH_SetUnitCollisionPattern(pUnit, COLLISION_PATTERN_SMALL_NO_PRESENCE);
                    PATH_SetFootprintCollisionMask(pUnit->pDynamicPath, COLLIDE_CORPSE);
                    return pUnit;
                }
            }
        }
    }

    return pUnit;
}

//D2Game.0x6FCBBB00
// 1.13c D2Game+0xE03A0 (checked against decompile, no delta)
D2UnitStrc* __fastcall SUNIT_GetServerUnit(D2GameStrc* pGame, int32_t nUnitType, int32_t nUnitGUID)
{
    D2_ASSERT(pGame);

    D2UnitStrc** ppUnitList = nullptr;

    if (nUnitType == UNIT_TILE)
    {
        ppUnitList = &pGame->pTileList;
    }
    else
    {
        ppUnitList = SUNIT_GetUnitList(nUnitType, pGame, nUnitGUID);
    }

    D2UnitStrc* pListHead = nullptr;
    pListHead = SUNIT_LoadPackedPointer(ppUnitList);
    for (D2UnitStrc* pUnit = pListHead; pUnit; pUnit = pUnit->pListNext)
    {
        if (pUnit->dwUnitId == nUnitGUID)
        {
            return pUnit;
        }
    }

    return nullptr;
}

//D2Game.0x6FCBBB70
// 1.13c D2Game+0xE0A20
D2UnitStrc* __fastcall SUNIT_GetOwner(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    if (!pUnit || !(pUnit->dwFlagEx & UNITFLAGEX_STOREOWNERINFO))
    {
        return nullptr;
    }
    
    return D2_CALL(SUNIT_GetServerUnit, pGame, pUnit->dwOwnerType, pUnit->dwOwnerGUID);
}

//D2Game.0x6FCBBC00
// 1.13c D2Game+0xE09D0 (unit in EAX, game on the stack; checked against disassembly, no delta)
D2UnitStrc* __fastcall SUNIT_GetLastAttacker(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    D2_ASSERT(pUnit);

    if (!(pUnit->dwFlagEx & UNITFLAGEX_STORELASTATTACKER))
    {
        return nullptr;
    }

    return D2_CALL(SUNIT_GetServerUnit, pGame, pUnit->dwKillerType, pUnit->dwKillerGUID);
}

// 1.13c D2Game+0xE0310 (row a retail recording): the sorted unit-list insert that 1.10f inlines at the end of SUNIT_Add.
// Convention EDX = pGame, ESI = pUnit, no stack words. The callee leaves the walk's stop node in EAX -- 0
// on an empty list or when the walk runs off the end -- and the FN vector records it, so it is the return
// value here; SUNIT_Add ignores it. 1.13c selects the slot from the D2Game+0xFB014 type table and reads it
// directly; SUNIT_GetUnitList is that same table.
D2UnitStrc* __fastcall SUNIT_AddUnitToList(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    D2UnitStrc** ppUnitList = pUnit->dwUnitType == UNIT_TILE ? &pGame->pTileList : SUNIT_GetUnitList(pUnit->dwUnitType, pGame, pUnit->dwUnitId);

    D2UnitStrc* pNode = *ppUnitList;
    D2UnitStrc* pPrevious = nullptr;
    while (pNode)
    {
        if (pNode->dwUnitId <= pUnit->dwUnitId)
        {
            if (pNode->dwUnitId == pUnit->dwUnitId)
            {
                FOG_DisplayHalt("Tried to add a duplicate unit", __FILE__, __LINE__);   // 1.13c: FOG_CallerReturnAddress(0x356) -> FOG_DisplayHalt("", caller, 0x356) -> _exit(-1)
                exit(-1);
            }
            break;
        }

        pPrevious = pNode;
        pNode = pNode->pListNext;
    }

    pUnit->pListNext = pNode;

    if (pPrevious)
    {
        pPrevious->pListNext = pUnit;
    }
    else
    {
        *ppUnitList = pUnit;
    }

    return pNode;
}

// 1.13c D2Game+0xDF9A0: the monster branch of SUNIT_Add asks it whether the unit's room has clients (1.10f inlines UNITS_GetRoom + nNumClients).
int32_t __fastcall SUNIT_RoomHasClients_DF9A0(D2UnitStrc* pUnit)
{
    D2ActiveRoomStrc* pCurrentRoom = UNITS_GetRoom(pUnit);
    return pCurrentRoom && pCurrentRoom->nNumClients;
}

//D2Game.0x6FCBBCB0
// 1.13c D2Game+0xE0F40: act/pDrlgAct from the room, per-type placement, sorted unit-list insert (E0310), refresh, invalidate-current-room when a6 == 0.
// Delta vs 1.10f: the monster branch tests the room's clients through D2Game+0xDF9A0 (SUNIT_RoomHasClients_DF9A0). Callees go through D2_CALL so a probe can record them.
void __fastcall SUNIT_Add(D2UnitStrc* pUnit, int32_t nX, int32_t nY, D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t a6)
{
    D2_ASSERT(pGame);
    D2_ASSERT(pUnit);

    pUnit->nAct = D2_CALL(DRLG_GetActNoFromLevelId, D2_CALL(DUNGEON_GetLevelIdFromRoom, pRoom));
    pUnit->pDrlgAct = pGame->pAct[pUnit->nAct];

    switch (pUnit->dwUnitType)
    {
    case UNIT_PLAYER:
    {
        D2_CALL(PATH_AllocDynamicPath, pGame->pMemoryPool, pRoom, nX, nY, pUnit, 0);
        D2_CALL(sub_6FC7B7A0, pGame, pUnit, nX, nY);
        break;
    }
    case UNIT_MONSTER:
    {
        D2_CALL(PATH_AllocDynamicPath, pGame->pMemoryPool, pRoom, nX, nY, pUnit, 0);
        D2_CALL(MONSTER_SetVelocityAndPosition, pGame, pUnit, nX, nY, a6);
        if (D2_CALL(SUNIT_RoomHasClients_DF9A0, pUnit))
        {
            D2_CALL(MONSTER_UpdateAiCallbackEvent, pGame, pUnit);
        }
        else
        {
            D2_CALL(sub_6FCBC300, pGame, pUnit);
        }
        break;
    }
    case UNIT_OBJECT:
    {
        D2ObjectsTxt* pObjectsTxtRecord = D2_CALL(DATATBLS_GetObjectsTxtRecord, pUnit->dwClassId);
        D2_CALL(UNITS_InitializeStaticPath, pUnit, pRoom, nX, nY);
        if (pObjectsTxtRecord->nHasCollision[pUnit->dwAnimMode])
        {
            D2_CALL(UNITS_BlockCollisionPath, pUnit, pRoom, nX, nY);
        }

        D2_ASSERT(pRoom);
        D2_CALL(UNITROOM_AddUnitToRoom, pUnit, pRoom);
        break;
    }
    case UNIT_MISSILE:
    {
        D2_CALL(PATH_AllocDynamicPath, pGame->pMemoryPool, pRoom, nX, nY, pUnit, 0);
        break;
    }
    case UNIT_ITEM:
    {
        if (pUnit->dwAnimMode == IMODE_ONGROUND)
        {
            D2_CALL(UNITS_InitializeStaticPath, pUnit, pRoom, nX, nY);
            D2_CALL(UNITS_BlockCollisionPath, pUnit, pRoom, nX, nY);

            if (pRoom)
            {
                D2_CALL(UNITROOM_AddUnitToRoomEx, pUnit, pRoom, 0);
            }
        }
        break;
    }
    case UNIT_TILE:
    {
        D2_CALL(UNITS_InitializeStaticPath, pUnit, pRoom, nX, nY);
        D2_CALL(UNITS_BlockCollisionPath, pUnit, pRoom, nX, nY);

        D2_ASSERT(pRoom);
        D2_CALL(UNITROOM_AddUnitToRoom, pUnit, pRoom);
        break;
    }
    default:
        FOG_DisplayAssert("0", __FILE__, __LINE__);
        exit(-1);
        return;
    }

    D2_CALL(SUNIT_AddUnitToList, pGame, pUnit);

    if (D2_CALL(UNITS_GetRoom, pUnit))
    {
        D2_CALL(UNITROOM_RefreshUnit, pUnit);
    }

    if (!a6)
    {
        D2_CALL(UNITS_SetCurrentRoomInvalid, pUnit, 1);
    }
}

//D2Game.0x6FCBBFE0
// 1.13c D2Game+0xE0D00 (checked against decompile, no delta; the room-has-clients test is D2Game+0xDF9A0, inline here)
void __fastcall SUNIT_Restore(D2GameStrc* pGame, D2UnitStrc* pUnit, D2ActiveRoomStrc* pRoom, int32_t nX, int32_t nY)
{
    D2_ASSERT(pGame);
    D2_ASSERT(pUnit);

    pUnit->nAct = DRLG_GetActNoFromLevelId(DUNGEON_GetLevelIdFromRoom(pRoom));
    pUnit->pDrlgAct = pGame->pAct[pUnit->nAct];
    pUnit->dwFlagEx &= ~UNITFLAGEX_HASBEENDELETED;

    switch (pUnit->dwUnitType)
    {
    case UNIT_PLAYER:
    {
        PATH_FreeDynamicPath(pGame->pMemoryPool, pUnit->pDynamicPath);
        pUnit->pDynamicPath = nullptr;

        PATH_AllocDynamicPath(pGame->pMemoryPool, pRoom, nX, nY, pUnit, 0);
        D2_CALL(sub_6FC7B7A0, pGame, pUnit, nX, nY);

        if (pUnit->dwAnimMode == PLRMODE_DEAD || pUnit->dwAnimMode == PLRMODE_DEATH)
        {
            PATH_RemoveCollisionFootprintForUnit(pUnit, 1);
            PATH_SetUnitCollisionPattern(pUnit, COLLISION_PATTERN_SMALL_NO_PRESENCE);
            PATH_SetFootprintCollisionMask(pUnit->pDynamicPath, COLLIDE_CORPSE);
        }
        break;
    }
    case UNIT_MONSTER:
    {
        PATH_FreeDynamicPath(pGame->pMemoryPool, pUnit->pDynamicPath);
        pUnit->pDynamicPath = nullptr;

        PATH_AllocDynamicPath(pGame->pMemoryPool, pRoom, nX, nY, pUnit, 0);
        D2_CALL(MONSTER_SetVelocityAndPosition, pGame, pUnit, nX, nY, 0);

        D2ActiveRoomStrc* pUnitRoom = D2_CALL(UNITS_GetRoom, pUnit);
        if (pUnitRoom && pUnitRoom->nNumClients)
        {
            D2_CALL(MONSTER_UpdateAiCallbackEvent, pGame, pUnit);
        }
        else
        {
            D2_CALL(sub_6FCBC300, pGame, pUnit);
        }

        if (MONSTERS_IsDead(pUnit))
        {
            PATH_RemoveCollisionFootprintForUnit(pUnit, 1);
            PATH_SetUnitCollisionPattern(pUnit, COLLISION_PATTERN_SMALL_NO_PRESENCE);
            PATH_SetFootprintCollisionMask(pUnit->pDynamicPath, COLLIDE_CORPSE);
        }
        break;
    }
    case UNIT_OBJECT:
    {
        D2ObjectsTxt* pObjectsTxtRecord = D2_CALL(DATATBLS_GetObjectsTxtRecord, pUnit->dwClassId);
        UNITS_InitializeStaticPath(pUnit, pRoom, nX, nY);

        if (pObjectsTxtRecord->nHasCollision[pUnit->dwAnimMode])
        {
            UNITS_BlockCollisionPath(pUnit, pRoom, nX, nY);
        }

        UNITROOM_AddUnitToRoom(pUnit, pRoom);
        break;
    }
    case UNIT_ITEM:
    {
        if (pUnit->dwAnimMode == IMODE_ONGROUND)
        {
            UNITS_InitializeStaticPath(pUnit, pRoom, nX, nY);
            UNITS_BlockCollisionPath(pUnit, pRoom, nX, nY);
            UNITROOM_AddUnitToRoom(pUnit, pRoom);
            pUnit->pItemData->dwActionStamp = ITEMS_GetGroundRemovalTime(pGame, pUnit);
        }
        break;
    }
    case UNIT_MISSILE:
    {
        PATH_FreeDynamicPath(pGame->pMemoryPool, pUnit->pDynamicPath);
        pUnit->pDynamicPath = nullptr;
        PATH_AllocDynamicPath(pGame->pMemoryPool, pRoom, nX, nY, pUnit, 0);
        break;
    }
    case UNIT_TILE:
    {
        UNITS_InitializeStaticPath(pUnit, pRoom, nX, nY);
        UNITS_BlockCollisionPath(pUnit, pRoom, nX, nY);
        UNITROOM_AddUnitToRoom(pUnit, pRoom);
        break;
    }
    default:
        FOG_DisplayHalt("Illegal unit type", __FILE__, __LINE__);
        exit(-1);
    }

    if (D2_CALL(UNITS_GetRoom, pUnit))
    {
        UNITROOM_RefreshUnit(pUnit);
    }

    UNITS_SetCurrentRoomInvalid(pUnit, 1);
}

// 1.13c D2Game+0xE0040 (diffed against the decompile: null unit and non-player unit are fatal, then the client pointer goes into the player data; no delta)
void __fastcall SUNIT_InitClientInPlayerData(D2UnitStrc* pUnit, D2ClientStrc* pClient)
{
    D2_ASSERT(pUnit);
    D2_ASSERT(pUnit->dwUnitType == UNIT_PLAYER);

    D2_CALL(UNITS_GetPlayerData, pUnit)->pClient = pClient;
}

// not-1.13c: retail has no function for this; the null/player-type test and the player-data client read are inlined at every call site (e.g. D2Game+0x8DFD0)
D2ClientStrc* __fastcall SUNIT_GetClientFromPlayer(D2UnitStrc* pPlayer, const char* szFile, int32_t nLine)
{
    if (!pPlayer || pPlayer->dwUnitType != UNIT_PLAYER)
    {
        return nullptr;
    }

    return D2_CALL(UNITS_GetPlayerData, pPlayer)->pClient;
}

//D2Game.0x6FCBC300
// 1.13c D2Game+0xE0860 (checked against the decompile: same clear masks, the double damage-reduction overlay check, per-type tail; no delta)
void __fastcall sub_6FCBC300(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    D2_ASSERT(pUnit);

    STATLIST_FreeModStats(pUnit);
    D2GAME_SUNITMSG_FreeUnitMessages_6FCC6790(pUnit);
    pUnit->dwFlags &= ~(UNITFLAG_DOUPDATE | UNITFLAG_INITSEEDSET);
    UNITS_SetCurrentRoomInvalid(pUnit, 0);
    pUnit->dwFlags &= ~(UNITFLAG_HASEVENTSOUND | UNITFLAG_UPGRLIFENHITCLASS);
    pUnit->dwFlagEx &= 0xFFDEE7FF;
    STATES_ClearGfxStateFlags(pUnit);
    D2GAME_INVMODE_Last_6FC416D0(pGame, pUnit);

    if (D2COMMON_10530_D2CheckStatlistFlagDMGRed(pUnit))
    {
        STATLIST_RemoveAllStatsFromOverlay(pUnit);
    }

    if (D2COMMON_10530_D2CheckStatlistFlagDMGRed(pUnit))
    {
        STATLIST_RemoveAllStatsFromOverlay(pUnit);
    }

    switch (pUnit->dwUnitType)
    {
    case UNIT_PLAYER:
    {
        STATLIST_SetUnitStat(pUnit, STAT_LASTEXP, -1, 0);
        pUnit->dwFlags &= ~UNITFLAG_HASTXTMSG;

        D2_ASSERT(pUnit && pUnit->dwUnitType == UNIT_PLAYER);

        D2ArenaUnitStrc* pArenaUnit = UNITS_GetPlayerData(pUnit)->pArenaUnit;

        D2_ASSERT(pArenaUnit);

        pArenaUnit->bUpdateScore = 0;
        break;
    }
    case UNIT_MONSTER:
    {
        pUnit->dwFlags &= ~UNITFLAG_HASTXTMSG;
        if (pUnit->dwFlags & UNITFLAG_SUMMONER)
        {
            pUnit->dwFlags &= ~UNITFLAG_SUMMONER;
            MONSTER_ToggleSummonerFlag(pUnit, 1, 0);
        }
        pUnit->dwFlagEx &= ~UNITFLAGEX_TELEPORTED;
        break;
    }
    case UNIT_OBJECT:
    {
        pUnit->dwFlags &= ~UNITFLAG_HASTXTMSG;
        break;
    }
    case UNIT_ITEM:
    {
        pUnit->dwFlags &= ~UNITFLAG_SENDREFRESHMSG;
        ITEMS_SetItemFlag(pUnit, IFLAG_QUANTITY, 0);
        ITEMS_SetItemFlag(pUnit, IFLAG_INSTORE, 0);
        break;
    }
    default:
        return;
    }
}

//D2Game.0x6FCBC480
// 1.13c D2Game+0xDFFD0
void __fastcall SUNIT_AttachSound(D2UnitStrc* pUnit, int32_t nUpdateType, D2UnitStrc* pUpdateUnit)
{
    D2_ASSERT(pUnit);

    pUnit->nUpdateType = nUpdateType;
    pUnit->pUpdateUnit = pUpdateUnit;
    D2_CALL(UNITROOM_RefreshUnit, pUnit);
    pUnit->dwFlags |= UNITFLAG_HASEVENTSOUND;
}

//D2Game.0x6FCBC4D0
// 1.13c D2Game+0xDFF10
void __fastcall sub_6FCBC4D0(D2UnitStrc* pUnit)
{
    D2_ASSERT(pUnit);

    pUnit->nActionFrame = 0;

    const bool bSequence = (pUnit->dwUnitType == UNIT_MONSTER && pUnit->dwAnimMode == MONMODE_SEQUENCE || pUnit->dwUnitType == UNIT_PLAYER && pUnit->dwAnimMode == PLRMODE_SEQUENCE);

    if (bSequence && DATATBLS_GetMonSeqTxtRecordFromUnit(pUnit))
    {
        UNITS_InitializeSequence(pUnit);
        UNITS_UpdateAnimRateAndVelocity(pUnit, __FILE__, __LINE__);
    }
    else
    {
        pUnit->pAnimSeq = 0;
        pUnit->nSeqCurrentFramePrecise = 0;
        if (pUnit->pDynamicPath)
        {
            UNITS_UpdateAnimRateAndVelocity(pUnit, __FILE__, __LINE__);
            pUnit->dwFrameCountPrecise = pUnit->pAnimData->dwFrames << 8;
        }
        else
        {
            UNITS_SetAnimData(pUnit, pUnit->dwUnitType, pUnit->dwClassId, pUnit->dwAnimMode);
            pUnit->dwFrameCountPrecise = pUnit->pAnimData->dwFrames << 8;
        }
    }
}

//D2Game.0x6FCBC590
// 1.13c D2Game+0xE12D0 (checked against the decompile: monster -> preset monster, object class 573 refused and above it the preset object, item -> init item stats, flags |= 0x3000000; nUnitFlags is not read in 1.13c; no other delta)
D2UnitStrc* __fastcall SUNIT_CreatePresetUnit(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nUnitType, int32_t nClassId, int32_t nX, int32_t nY, int32_t nMode, int32_t nUnitFlags)
{
    D2UnitStrc* pUnit = nullptr;
    if (nUnitType == UNIT_MONSTER)
    {
        pUnit = D2_CALL(D2GAME_SpawnPresetMonster_6FC66560, pGame, pRoom, nClassId, nX, nY, nMode);
    }
    else if (nUnitType == UNIT_OBJECT)
    {
        if (nClassId == 573)
        {
            FOG_Trace(" could not place object due to invalid preset data\n");
            return nullptr;
        }

        if (nClassId > 573)
        {
            pUnit = D2_CALL(OBJECTS_SpawnPresetObject, pGame, pRoom, nClassId, nX, nY, nMode);
        }
        else
        {
            pUnit = SUNIT_AllocUnitData(nUnitType, nClassId, nX, nY, pGame, pRoom, 1, nMode, 0);
        }
    }
    else
    {
        pUnit = SUNIT_AllocUnitData(nUnitType, nClassId, nX, nY, pGame, pRoom, 1, nMode, 0);

        if (nUnitType == UNIT_ITEM)
        {
            if (!pUnit)
            {
                return nullptr;
            }

            D2ItemDropStrc itemDrop = {};
            itemDrop.nQuality = ITEMQUAL_NORMAL;
            itemDrop.nItemLvl = DUNGEON_GetLevelIdFromRoom(pRoom);
            if (!D2GAME_InitItemStats_6FC4E520(pGame, &pUnit, &itemDrop, 1))
            {
                SUNIT_RemoveUnit(pGame, pUnit);
                return nullptr;
            }
        }
    }

    if (pUnit)
    {
        pUnit->dwFlags |= UNITFLAG_ISRESURRECT | UNITFLAG_ISINIT;
    }

    return pUnit;
}

//D2Game.0x6FCBC6F0
// 1.13c D2Game+0xE13B0
void __fastcall SUNIT_SpawnPresetUnit(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, D2PresetUnitStrc* pPresetUnit)
{
    D2DrlgCoordsStrc pCoord = {};

    DUNGEON_GetRoomCoordinates(pRoom, &pCoord);

    D2UnitStrc* pUnit = SUNIT_CreatePresetUnit(pGame, pRoom, pPresetUnit->nUnitType, pPresetUnit->nIndex, pCoord.nSubtileX + pPresetUnit->nXpos, pCoord.nSubtileY + pPresetUnit->nYpos, pPresetUnit->nMode, 0);
    if (!pUnit)
    {
        return;
    }

    if (pPresetUnit->nUnitType == UNIT_MONSTER)
    {
        D2MapAIStrc** ppMapAI = AIGENERAL_GetMapAiFromUnit(pUnit);
        if (pPresetUnit->pMapAI)
        {
            DRLGPRESET_ChangeMapAI(&pPresetUnit->pMapAI, ppMapAI);
        }
    }
    else if (pPresetUnit->nUnitType == UNIT_OBJECT)
    {
        QUESTS_MapPresetSpawn(pGame, pUnit, pPresetUnit->pMapAI);
    }
}

//1.10: D2Game.0x6FCBC780
//1.14d: 0x005559A0
// 1.13c D2Game+0x6FD01430 replay seam (kind 3452, s3255): stands in for the three retail callees (A664 presets, A556 level, E13B0 spawn); null in the game.
struct D2Gate1430Seam { D2PresetUnitStrc* (*presets)(D2ActiveRoomStrc*); int32_t (*level)(D2ActiveRoomStrc*); void (*spawn)(D2GameStrc*, D2ActiveRoomStrc*, D2PresetUnitStrc*); };
D2Gate1430Seam g_d2Gate1430 = {};
// 1.13c D2Game+0xE1430
void __fastcall SUNIT_SpawnPresetUnitsInRoom(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom)
{
    if (!pGame)
    {
        return;
    }

    const D2Gate1430Seam& z = g_d2Gate1430;
    D2PresetUnitStrc* pPresetUnit = z.presets ? z.presets(pRoom) : DUNGEON_GetPresetUnitsFromRoom(pRoom);
#ifdef D2_MONS_TRACE
    if (DUNGEON_GetLevelIdFromRoom(pRoom) == d2_trace_level) for (D2PresetUnitStrc* q = pPresetUnit; q; q = q->pNext) fprintf(stderr, "  preset type %d index %d at (%d,%d) mode %d spawned %d\n", q->nUnitType, q->nIndex, q->nXpos, q->nYpos, q->nMode, q->bSpawned);
#endif
#ifdef D2_VERSION_HAS_UBERS
    // d2-web: 1.13c 0x6FD01430. The uber levels only filter presets here (the bosses are spawned
    // by the portal/chest events, not by room activation): Matron's Den / Forgotten Sands /
    // Furnace of Pain skip the sparkly chest (Forgotten Sands additionally drops the
    // (dwInitSeed % 3 + 3)-th preset, counted across the level in pGame->dwUberSandsCounter);
    // Tristram (finale) skips Wirt's body, the gibbet and the gold placeholder.
    const int32_t nLevelId = z.level ? z.level(pRoom) : DUNGEON_GetLevelIdFromRoom(pRoom);
    for (D2PresetUnitStrc* i = pPresetUnit; i; i = i->pNext)
    {
        if (i->nUnitType == UNIT_MONSTER || (i->bSpawned & 1))
        {
            continue;
        }

        if (nLevelId == LEVEL_PANDEMONIUMFINALE)
        {
            if (i->nUnitType == UNIT_OBJECT && (i->nIndex == OBJECT_WIRTSBODY || i->nIndex == OBJECT_CAINGIBBET || i->nIndex == OBJECT_GOLD_PLACEHOLDER))
            {
                continue;
            }
            (z.spawn ? z.spawn : SUNIT_SpawnPresetUnit)(pGame, pRoom, i);
        }
        else if (nLevelId == LEVEL_PANDEMONIUMRUN1 || nLevelId == LEVEL_PANDEMONIUMRUN3)
        {
            if (i->nUnitType == UNIT_OBJECT && i->nIndex == OBJECT_SPARKLY_CHEST)
            {
                continue;
            }
            (z.spawn ? z.spawn : SUNIT_SpawnPresetUnit)(pGame, pRoom, i);
        }
        else if (nLevelId == LEVEL_PANDEMONIUMRUN2)
        {
            // 1.13c 0xE1430 (kind 3452): only objects are counted; any other preset type spawns without touching the counter.
            if (i->nUnitType == UNIT_OBJECT)
            {
                if (i->nIndex == OBJECT_VALLEY_WAYPOINT)
                {
                    continue;
                }
                if (++pGame->dwUberSandsCounter == pGame->dwInitSeed % 3 + 3)
                {
                    continue;
                }
            }
            (z.spawn ? z.spawn : SUNIT_SpawnPresetUnit)(pGame, pRoom, i);
        }
        else
        {
            (z.spawn ? z.spawn : SUNIT_SpawnPresetUnit)(pGame, pRoom, i);
        }
    }

    if (nLevelId != LEVEL_PANDEMONIUMFINALE)
    {
        for (D2PresetUnitStrc* i = pPresetUnit; i; i = i->pNext)
        {
            if (i->nUnitType == UNIT_MONSTER && !(i->bSpawned & 1))
            {
                (z.spawn ? z.spawn : SUNIT_SpawnPresetUnit)(pGame, pRoom, i);
            }
        }
    }
#else
    for (D2PresetUnitStrc* i = pPresetUnit; i; i = i->pNext)
    {
        if (i->nUnitType != UNIT_MONSTER && !(i->bSpawned & 1))
        {
            (z.spawn ? z.spawn : SUNIT_SpawnPresetUnit)(pGame, pRoom, i);
        }
    }

    for (D2PresetUnitStrc* i = pPresetUnit; i; i = i->pNext)
    {
        if (i->nUnitType == UNIT_MONSTER && !(i->bSpawned & 1))
        {
            (z.spawn ? z.spawn : SUNIT_SpawnPresetUnit)(pGame, pRoom, i);
        }
    }
#endif
}

//D2Game.0x6FCBC7E0
// 1.13c D2Game+0xE07A0
void __fastcall sub_6FCBC7E0(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    D2_ASSERT(pUnit);
    D2_ASSERT(pGame);

    D2DynamicPathStrc* pPath = pUnit->pDynamicPath;
    D2_ASSERT(pPath);

    if (!PATH_GetTargetUnit(pUnit->pDynamicPath))
    {
        return;
    }

    int32_t nTargetType = 0;
    D2UnitGUID nTargetGUID = 0;
    PATH_GetTargetTypeAndGUID(pPath, &nTargetType, &nTargetGUID);

    D2UnitStrc* pTarget = SUNIT_GetServerUnit(pGame, nTargetType, nTargetGUID);
    if (pTarget != PATH_GetTargetUnit(pPath))
    {
        PATH_SetTargetUnit(pPath, nullptr);
    }

    if (!pTarget || pTarget->dwUnitType != UNIT_ITEM)
    {
        return;
    }

    const int32_t nAnimMode = pTarget->dwAnimMode;
    if (nAnimMode == IMODE_INBELT || nAnimMode == IMODE_EQUIP)
    {
        PATH_SetTargetUnit(pPath, nullptr);
    }
}

//D2Game.0x6FCBC900
// 1.13c D2Game+0xE0BD0
// 1.13c D2Game+0xE0BD0
D2UnitStrc* __stdcall SUNIT_GetTargetUnit(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    D2_CALL(sub_6FCBC7E0, pGame, pUnit);
    return SUNIT_TargetOrNull(pUnit, D2_CALL(PATH_GetTargetUnit, pUnit->pDynamicPath));
}

//D2Game.0x6FCBC930
// 1.13c D2Game+0xE1930
int32_t __fastcall sub_6FCBC930(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2_ASSERT(pGame);

    sub_6FCBC7E0(pGame, pMissile);
    const int32_t v4 = 2 - (D2Common_10226(pMissile, 1024) != 0);
    if (UNITS_IsCurrentRoomInvalid(pMissile))
    {
        sub_6FCBB190(pGame, pMissile, 0);
    }

    return v4;
}

//D2Game.0x6FCBC9A0
// 1.13c D2Game+0xDF980
void __fastcall SUNIT_SetCombatMode(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nMode)
{
    SUNIT_SetCombatModeSteps([&] { D2_CALL(SUNITDMG_FreeAttackerCombatList, pGame, pUnit); }, [&] { D2_CALL(UNITS_ChangeAnimMode, pUnit, nMode); });
}

//D2Game.0x6FCBC9C0
// 1.13c D2Game+0xDFDC0 (first in EDI, second in ESI; the unit X/Y reads are inline; checked against disassembly, no delta)
void __fastcall sub_6FCBC9C0(D2UnitStrc* pFirst, D2UnitStrc* pSecond)
{
    D2_ASSERT(pFirst);
    D2_ASSERT(pSecond);

    pFirst->pObjectData->DestPortalCoords.nX = CLIENTS_GetUnitX(pSecond);
    pFirst->pObjectData->DestPortalCoords.nY = CLIENTS_GetUnitY(pSecond);

    pSecond->pObjectData->DestPortalCoords.nX = CLIENTS_GetUnitX(pFirst);
    pSecond->pObjectData->DestPortalCoords.nY = CLIENTS_GetUnitY(pFirst);

    D2ActiveRoomStrc* pSecondRoom = UNITS_GetRoom(pSecond);
    if (pSecondRoom)
    {
        D2DrlgCoordsStrc drlgCoords = {};
        DUNGEON_GetRoomCoordinates(pSecondRoom, &drlgCoords);
        pFirst->pObjectData->DestRoomCooords.nX = drlgCoords.nTileXPos;
        pFirst->pObjectData->DestRoomCooords.nY = drlgCoords.nTileYPos;
    }

    D2ActiveRoomStrc* pFirstRoom = UNITS_GetRoom(pFirst);
    if (pFirstRoom)
    {
        D2DrlgCoordsStrc drlgCoords = {};
        DUNGEON_GetRoomCoordinates(pFirstRoom, &drlgCoords);
        pSecond->pObjectData->DestRoomCooords.nX = drlgCoords.nTileXPos;
        pSecond->pObjectData->DestRoomCooords.nY = drlgCoords.nTileYPos;
    }
}

//D2Game.0x6FCBCB30
// 1.13c D2Game+0xE1570 (stdcall game, portal; checked against disassembly, no delta)
D2UnitStrc* __fastcall SUNIT_GetPortalOwner(D2GameStrc* pGame, D2UnitStrc* pPortal)
{
    D2_ASSERT(pPortal);

    const uint8_t nActNo = DRLG_GetActNoFromLevelId(pPortal->pObjectData->InteractType);
    const int32_t nX = pPortal->pObjectData->DestRoomCooords.nX;
    const int32_t nY = pPortal->pObjectData->DestRoomCooords.nY;

    if (!DUNGEON_FindRoomBySubtileCoordinates(pGame->pAct[nActNo], nX, nY))
    {
        D2ActiveRoomStrc* pRoom = DUNGEON_StreamRoomAtCoords(pGame->pAct[nActNo], nX, nY);
        if (pRoom)
        {
            sub_6FC385A0(pGame, pRoom);
        }
    }

    return SUNIT_GetServerUnit(pGame, pPortal->dwOwnerType, pPortal->dwOwnerGUID);
}

//D2Game.0x6FCBCC40
// 1.13c D2Game+0xDFCC0 (checked against decompile, no delta; register-convention wrapper)
void __fastcall SUNIT_IterateUnitsOfType(D2GameStrc* pGame, int32_t nType, void* pArg, int32_t(__fastcall* pfIterate)(D2GameStrc*, D2UnitStrc*, void*))
{
    D2_ASSERT(pGame);
    D2_ASSERT(pfIterate);

    D2UnitStrc** ppUnitList = nullptr;
    if (nType == UNIT_PLAYER)
    {
        ppUnitList = pGame->pUnitList[0];
    }
    else if (nType == UNIT_MONSTER)
    {
        ppUnitList = pGame->pUnitList[1];
    }
    else
    {
        return;
    }

    if (IsBadCodePtr((FARPROC)pfIterate))
    {
        FOG_DisplayAssert("pfnIterate", __FILE__, __LINE__);
        exit(-1);
    }

    for (int32_t i = 0; i < 128; ++i)
    {
        for (D2UnitStrc* pUnit = ppUnitList[i]; pUnit; pUnit = pUnit->pListNext)
        {
            if (!STATES_CheckState(pUnit, STATE_PLAYERBODY) && pfIterate(pGame, pUnit, pArg) == 1)
            {
                return;
            }

            D2_ASSERT(pUnit);
        }
    }
}

//D2Game.0x6FCBCD70
// 1.13c D2Game+0xDFC10 (checked against decompile; register-convention wrapper, no semantic delta)
void __fastcall SUNIT_IterateLivingPlayers(D2GameStrc* pGame, void(__fastcall* pfIterate)(D2GameStrc*, D2UnitStrc*, void*), void* pArg)
{
    D2_ASSERT(pGame && pfIterate);

    for (int32_t i = 0; i < 128; ++i)
    {
        for (D2UnitStrc* pUnit = pGame->pUnitList[0][i]; pUnit; pUnit = pUnit->pListNext)
        {
            if (!STATES_CheckState(pUnit, STATE_PLAYERBODY))
            {
                pfIterate(pGame, pUnit, pArg);
            }

            D2_ASSERT(pUnit);
        }
    }
}

// not-1.13c: retail has no function for this; the event code writes the unit's timer pointer in place
void __fastcall SUNIT_SetTimerOnUnit(D2UnitStrc* pUnit, D2EventTimerStrc* pTimer)
{
    if (pUnit)
    {
        pUnit->pTimer = pTimer;
    }
}

// not-1.13c: retail has no function for this; the event code reads the unit's timer pointer in place
D2EventTimerStrc* __fastcall SUNIT_GetTimerFromUnit(D2UnitStrc* pUnit)
{
    if (pUnit)
    {
        return pUnit->pTimer;
    }

    return nullptr;
}

//D2Game.0x6FCBCE50
// 1.13c D2Game+0xE0750
void __fastcall D2GAME_DeletePlayerPerFrameEvents_6FCBCE50(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    D2GAME_EVENTS_Delete_6FC34840(pGame, pUnit, EVENTTYPE_MODECHANGE, 0);
    D2GAME_EVENTS_Delete_6FC34840(pGame, pUnit, EVENTTYPE_ENDANIM, 0);
}

//D2Game.0x6FCBCE70
// 1.13c D2Game+0xDF7B0
void __fastcall sub_6FCBCE70(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    int32_t nFrame = pGame->dwGameFrame;
    const int32_t nFrameBonus = D2_CALL(UNITS_GetFrameBonus, pUnit) << 8;

    int32_t nAnimSpeed = 0;
    int32_t nFrameCount = 0;
    if (pUnit->pAnimSeq)
    {
        nAnimSpeed = pUnit->dwSeqSpeed;
        nFrameCount = pUnit->dwSeqFrameCount;
    }
    else
    {
        nAnimSpeed = pUnit->wAnimSpeed;
        nFrameCount = pUnit->dwFrameCountPrecise;
    }

    if (nAnimSpeed)
    {
        int32_t nIndex = nFrameBonus >> 8;
        int32_t nParam = 0;
        int32_t nAnimFrame = nAnimSpeed + nFrameBonus;
        while (nAnimFrame < nFrameCount)
        {
            ++nFrame;
            for (int32_t i = nAnimFrame >> 8; nIndex <= i; ++nIndex)
            {
                if (nIndex >= 144)
                {
                    break;
                }

                D2AnimSeqTxt* pSequence = (D2AnimSeqTxt*)pUnit->pAnimSeq;
                int32_t nEvent = 0;
                if (pSequence)
                {
                    D2_CALL(DATATBLS_GetSequenceEvent, pSequence, nIndex << 8, &nEvent);
                }
                else
                {
                    nEvent = pUnit->pAnimData->pFrameFlags[nIndex];
                }

                switch (nEvent)
                {
                case 1:
                case 2:
                case 4:
                {
                    D2_CALL(EVENT_SetEvent, pGame, pUnit, EVENTTYPE_MODECHANGE, nFrame, nEvent, nParam);
                    ++nParam;
                    break;
                }
                case 3:
                {
                    D2_CALL(EVENT_SetEvent, pGame, pUnit, EVENTTYPE_MODECHANGE, nFrame, nEvent, 0);
                    break;
                }
                default:
                    break;
                }
            }

            nAnimFrame += nAnimSpeed;
        }

        if (nFrame == pGame->dwGameFrame)
        {
            ++nFrame;
        }

        D2_CALL(EVENT_SetEvent, pGame, pUnit, EVENTTYPE_ENDANIM, nFrame + 1, 0, 0);
        pUnit->nSeqCurrentFramePrecise = pGame->dwGameFrame << 8;
    }
    else
    {
        D2_CALL(EVENT_SetEvent, pGame, pUnit, EVENTTYPE_ENDANIM, pGame->dwGameFrame + 1, 0, 0);
    }
}

//D2Game.0x6FCBCFD0
// 1.13c D2Game+0xE2600 (unit in ESI; the two event deletes are inline walks of unit+0xDC; checked against decompile, no delta)
void __fastcall sub_6FCBCFD0(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t a3)
{
    D2GAME_EVENTS_Delete_6FC34840(pGame, pUnit, EVENTTYPE_MODECHANGE, 0);
    D2GAME_EVENTS_Delete_6FC34840(pGame, pUnit, EVENTTYPE_ENDANIM, 0);

    const int32_t nCalc = (100 - a3) * (pGame->dwGameFrame - (pUnit->nSeqCurrentFramePrecise >> 8)) / 100;
    pUnit->nSeqCurrentFramePrecise = (pGame->dwGameFrame - nCalc) << 8;

    int32_t nAnimSpeed = 0;
    int32_t nFrameCount = 0;
    if (pUnit->pAnimSeq)
    {
        nAnimSpeed = pUnit->dwSeqSpeed;
        nFrameCount = pUnit->dwSeqFrameCount;
    }
    else
    {
        nAnimSpeed = pUnit->wAnimSpeed;
        nFrameCount = pUnit->dwFrameCountPrecise;
    }

    if (nAnimSpeed)
    {
        int32_t nFrame = pGame->dwGameFrame;
        int32_t nIndex = nCalc - 1;
        int32_t nAnimFrame = (nCalc << 8) + nAnimSpeed;
        while (nAnimFrame < nFrameCount)
        {
            ++nFrame;

            int32_t nEvent = 0;
            while (nIndex <= (nAnimFrame >> 8))
            {
                D2AnimSeqTxt* pSequence = (D2AnimSeqTxt*)pUnit->pAnimSeq;
                if (pSequence)
                {
                    DATATBLS_GetSequenceEvent(pSequence, nIndex << 8, &nEvent);
                }
                else
                {
                    nEvent = pUnit->pAnimData->pFrameFlags[nIndex];
                }

                if (nEvent > 0 && nEvent <= 4)
                {
                    EVENT_SetEvent(pGame, pUnit, EVENTTYPE_MODECHANGE, nFrame, nEvent, 0);
                }

                ++nIndex;
            }

            nAnimFrame += nAnimSpeed;
        }

        if (nFrame == pGame->dwGameFrame)
        {
            ++nFrame;
        }

        EVENT_SetEvent(pGame, pUnit, EVENTTYPE_ENDANIM, nFrame + 1, 0, 0);
    }
    else
    {
        EVENT_SetEvent(pGame, pUnit, EVENTTYPE_ENDANIM, pGame->dwGameFrame + 1, 0, 0);
    }
}

//D2Game.0x6FCBD120
// 1.13c D2Game+0xE2480
void __fastcall sub_6FCBD120(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t a3)
{
    if (a3 <= 0)
    {
        return;
    }

    D2GAME_EVENTS_Delete_6FC34840(pGame, pUnit, EVENTTYPE_MODECHANGE, 0);
    D2GAME_EVENTS_Delete_6FC34840(pGame, pUnit, EVENTTYPE_ENDANIM, 0);

    const int32_t nCalc = pGame->dwGameFrame - (pUnit->nSeqCurrentFramePrecise >> 8) - a3;
    pUnit->nSeqCurrentFramePrecise = (pGame->dwGameFrame - nCalc) << 8;

    int32_t nAnimSpeed = 0;
    int32_t nFrameCount = 0;
    if (pUnit->pAnimSeq)
    {
        nAnimSpeed = pUnit->dwSeqSpeed;
        nFrameCount = pUnit->dwSeqFrameCount;
    }
    else
    {
        nAnimSpeed = pUnit->wAnimSpeed;
        nFrameCount = pUnit->dwFrameCountPrecise;
    }

    if (nAnimSpeed)
    {
        int32_t nFrame = pGame->dwGameFrame;
        int32_t nIndex = nCalc - 1;
        int32_t nAnimFrame = (nCalc << 8) + nAnimSpeed;
        while (nAnimFrame < nFrameCount)
        {
            ++nFrame;

            int32_t nEvent = 0;
            while (nIndex <= (nAnimFrame >> 8))
            {
                D2AnimSeqTxt* pSequence = (D2AnimSeqTxt*)pUnit->pAnimSeq;
                if (pSequence)
                {
                    DATATBLS_GetSequenceEvent(pSequence, nIndex << 8, &nEvent);
                }
                else
                {
                    nEvent = pUnit->pAnimData->pFrameFlags[nIndex];
                }

                if (nEvent > 0 && nEvent <= 4)
                {
                    EVENT_SetEvent(pGame, pUnit, EVENTTYPE_MODECHANGE, nFrame, nEvent, 0);
                }

                ++nIndex;
            }

            nAnimFrame += nAnimSpeed;
        }

        if (nFrame == pGame->dwGameFrame)
        {
            ++nFrame;
        }

        EVENT_SetEvent(pGame, pUnit, EVENTTYPE_ENDANIM, nFrame + 1, 0, 0);
    }
    else
    {
        EVENT_SetEvent(pGame, pUnit, EVENTTYPE_ENDANIM, pGame->dwGameFrame + 1, 0, 0);
    }
}

//D2Game.0x6FCBD260
// 1.13c D2Game+0xE2310 (unit in EBX; the two event deletes are inline walks of unit+0xDC; checked against decompile, no delta)
void __fastcall D2GAME_SKILLS_RewindSkillEx_6FCBD260(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t a3)
{
    D2GAME_EVENTS_Delete_6FC34840(pGame, pUnit, EVENTTYPE_MODECHANGE, 0);
    D2GAME_EVENTS_Delete_6FC34840(pGame, pUnit, EVENTTYPE_ENDANIM, 0);

    pUnit->nSeqCurrentFramePrecise = (pGame->dwGameFrame - a3) << 8;

    int32_t nAnimSpeed = 0;
    int32_t nFrameCount = 0;
    if (pUnit->pAnimSeq)
    {
        nAnimSpeed = pUnit->dwSeqSpeed;
        nFrameCount = pUnit->dwSeqFrameCount;
    }
    else
    {
        nAnimSpeed = pUnit->wAnimSpeed;
        nFrameCount = pUnit->dwFrameCountPrecise;
    }

    if (nAnimSpeed)
    {
        int32_t nFrame = pGame->dwGameFrame;
        int32_t nIndex = a3 - 1;
        int32_t nAnimFrame = (a3 << 8) + nAnimSpeed;
        while (nAnimFrame < nFrameCount)
        {
            while (nIndex <= (nAnimFrame >> 8))
            {
                D2AnimSeqTxt* pSequence = (D2AnimSeqTxt*)pUnit->pAnimSeq;
                int32_t nEvent = 0;
                if (pSequence)
                {
                    DATATBLS_GetSequenceEvent(pSequence, nIndex << 8, &nEvent);
                }
                else
                {
                    nEvent = pUnit->pAnimData->pFrameFlags[nIndex];
                }

                if (nEvent > 0 && nEvent <= 4)
                {
                    EVENT_SetEvent(pGame, pUnit, EVENTTYPE_MODECHANGE, nFrame + 1, nEvent, 0);
                }

                ++nIndex;
            }

            nAnimFrame += nAnimSpeed;
            ++nFrame;
        }

        if (nFrame == pGame->dwGameFrame)
        {
            ++nFrame;
        }

        EVENT_SetEvent(pGame, pUnit, EVENTTYPE_ENDANIM, nFrame + 1, 0, 0);
    }
    else
    {
        EVENT_SetEvent(pGame, pUnit, EVENTTYPE_ENDANIM, pGame->dwGameFrame + 1, 0, 0);
    }
}

//D2Game.0x6FCBD3A0
// 1.13c D2Game+0xE0B40
void __fastcall sub_6FCBD3A0(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    D2GAME_EVENTS_Delete_6FC34840(pGame, pUnit, EVENTTYPE_MODECHANGE, 0);
    sub_6FC35570(pGame, pUnit, EVENTTYPE_MODECHANGE, nullptr, 0, 0);
}

//D2Game.0x6FCBD3D0
// 1.13c D2Game+0xDFB30 (checked against decompile; identical)
void __fastcall SUNIT_FillUnitInfo(D2UnitStrc* pUnit, D2UnitInfoStrc* pInfo)
{
    D2_ASSERT(pInfo);
    D2_ASSERT(pUnit);

    pInfo->szDescription[0] = 0;
    if (pUnit->dwUnitType == UNIT_PLAYER)
    {
        D2PlayerDataStrc* pPlayerData = D2_CALL(UNITS_GetPlayerData, pUnit);
        if (pPlayerData)
        {
            strcpy_s(pInfo->szDescription, pPlayerData->szName);
        }
    }

    pInfo->nUnitType = pUnit->dwUnitType;
    pInfo->nClassId = pUnit->dwClassId;
    pInfo->nUnitGUID = pUnit->dwUnitId;
    pInfo->nAnimMode = pUnit->dwAnimMode;
    pInfo->nX = D2_CALL(CLIENTS_GetUnitX, pUnit);
    pInfo->nY = D2_CALL(CLIENTS_GetUnitY, pUnit);
    pInfo->nOwnerType = pUnit->dwOwnerType;
    pInfo->nOwnerGUID = pUnit->dwOwnerGUID;
}

//D2Game.0x6FCBD4D0
// not-1.13c: retail inlines this at each use (byte test of unit+0x6C, then +0xE03A0 with type +0x68 / GUID +0x64; e.g. D2Game+0x6A7AD, +0x9B6A8)
D2UnitStrc* __fastcall SUNIT_GetInteractUnit(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    if (!*reinterpret_cast<uint8_t*>(&pUnit->nInteract))
    {
        return nullptr;
    }

    return SUNIT_GetServerUnit(pGame, pUnit->dwInteractType, pUnit->dwInteractGUID);
}

//D2Game.0x6FCBD550
// 1.13c D2Game+0xE1110 (checked against decompile; retail's page-3/charm calls are no-op ordinals, left out)
void __fastcall SUNIT_RemoveAllItemsFromInventory(D2GameStrc* pGame, D2UnitStrc* pUnit)
{
    D2_ASSERT(pUnit);

    D2InventoryStrc* pInventory = pUnit->pInventory;
    if (!pInventory)
    {
        return;
    }

    D2UnitStrc* i = INVENTORY_GetFirstItem(pInventory);
    while (i)
    {
        D2UnitStrc* pNext = INVENTORY_GetNextItem(i);

        D2UnitStrc* pItem = INVENTORY_UnitIsItem(i);
        D2_ASSERT(pItem);

        if (pItem->dwUnitType == UNIT_ITEM)
        {
            D2GAME_ITEMS_UpdateTransferredProperties_6FC424E0(pGame, pItem, pUnit, 0, 1);

            //if (INVENTORY_GetItemNodePage(i) == 3)
            //{
            //    D2COMMON_10254_Return0(pInventory, INVENTORY_GetBodyLocFromEquippedItem(pInventory, pItem));
            //}
            //else if (ITEMS_IsCharmUsable(pItem, pUnit))
            //{
            //    const uint8_t nPage = ITEMS_GetInvPage(pItem);
            //    INVENTORY_Return(__FILE__, __LINE__, pInventory, CLIENTS_GetUnitX(pItem), CLIENTS_GetUnitY(pItem), UNITS_GetInventoryRecordId(pUnit, nPage, pGame->bExpansion), 0, nPage);
            //}
        }

        if (!INVENTORY_RemoveItemFromInventory(pInventory, pItem))
        {
            FOG_DisplayAssert("hRemovedItem", __FILE__, __LINE__);
            exit(-1);
        }

        if (pItem->dwUnitType < 6)
        {
            SUNIT_RemoveUnit(pGame, pItem);
        }

        i = pNext;
    }

    D2UnitStrc* pCursorItem = INVENTORY_GetCursorItem(pInventory);
    if (pCursorItem && pCursorItem->dwUnitType < 6)
    {
        SUNIT_RemoveUnit(pGame, pCursorItem);
    }
}

//D2Game.0x6FCBD730
// 1.13c D2Game+0xDF780
D2GameStrc* __fastcall SUNIT_GetGameFromUnit(D2UnitStrc* pUnit)
{
    D2_ASSERT(pUnit);

    return pUnit->pGame;
}

//D2Game.0x6FCBD760
// 1.13c D2Game+0xDF750 (unit in EAX; player data +0x40)
void __fastcall D2GAME_SetNecropetFlag_6FCBD760(D2UnitStrc* pUnit, int32_t nFlag)
{
    //TODO: pUnit == PLAYER or MONSTER?
    D2_ASSERT(pUnit);

    pUnit->pPlayerData->unk0x3C[1] = nFlag;
}

//D2Game.0x6FCBD790
// 1.13c D2Game+0xDF720 (unit in EAX; player data +0x40)
uint32_t __fastcall D2GAME_GetNecropetFlag_6FCBD790(D2UnitStrc* pUnit)
{
    //TODO: pUnit == PLAYER or MONSTER?
    D2_ASSERT(pUnit);

    return pUnit->pPlayerData->unk0x3C[1];
}

//D2Game.0x6FCBD7C0
// 1.13c D2Game+0xDF6F0 (unit in EAX; writes one byte at +0x78)
void __fastcall D2GAME_SetSparkChest_6FCBD7C0(D2UnitStrc* pUnit, uint8_t nSparkChest)
{
    D2_ASSERT(pUnit);

    *reinterpret_cast<uint8_t*>(&pUnit->bSparkChest) = nSparkChest;
}

//D2Game.0x6FCBD7F0
// 1.13c D2Game+0xDF6C0 (unit in EAX; reads one byte at +0x78)
uint8_t __fastcall D2GAME_CheckIfSparklyChest_6FCBD7F0(D2UnitStrc* pUnit)
{
    D2_ASSERT(pUnit);

    return *reinterpret_cast<uint8_t*>(&pUnit->bSparkChest);
}

//D2Game.0x6FCBD820
// 1.13c D2Game+0xDF6A0 (unit in EAX, GUID out in EDX, type out on the stack; returns the byte at +0x6C)
int32_t __stdcall SUNIT_GetInteractInfo(D2UnitStrc* pUnit, int32_t* pInteractUnitType, int32_t* pInteractUnitGUID)
{
    *pInteractUnitGUID = pUnit->dwInteractGUID;
    *pInteractUnitType = pUnit->dwInteractType;
    return *reinterpret_cast<uint8_t*>(&pUnit->nInteract);
}

//D2Game.0x6FCBD840
// 1.13c D2Game+0xDFAE0
void __stdcall SUNIT_SetInteractInfo(D2UnitStrc* pUnit, int32_t nInteractUnitType, int32_t nInteractUnitGUID)
{
    if (pUnit->nInteract)
    {
        if (pUnit->dwUnitType == UNIT_PLAYER)
        {
            D2PlayerDataStrc* pPlayerData = UNITS_GetPlayerData(pUnit);
            if (pPlayerData)
            {
                char szMessage[80] = {};
                sprintf(szMessage, "Player %s attempted to operate while busy\n", pPlayerData->szName);
            }
        }
    }
    else
    {
        pUnit->dwInteractGUID = nInteractUnitGUID;
        pUnit->dwInteractType = nInteractUnitType;
        pUnit->nInteract = 1;
    }
}

//D2Game.0x6FCBD890
// 1.13c D2Game+0xDF680 (unit in EAX)
void __stdcall SUNIT_ResetInteractInfo(D2UnitStrc* pUnit)
{
    pUnit->dwInteractGUID = -1;
    pUnit->dwInteractType = 6;
    *reinterpret_cast<uint8_t*>(&pUnit->nInteract) = 0; // 1.13c writes one byte at +0x6C
}

//D2Game.0x6FCBD8B0
// 1.13c D2Game+0xDFA90
int32_t __fastcall SUNIT_IsDead(D2UnitStrc* pUnit)
{
    if (!pUnit || pUnit->dwFlags & 0x10000)
    {
        return 1;
    }

    if (pUnit->dwUnitType == UNIT_PLAYER)
    {
        if (pUnit->dwAnimMode == PLRMODE_DEATH || pUnit->dwAnimMode == PLRMODE_DEAD)
        {
            return 1;
        }
        return 0;
    }
    else if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        if (pUnit->dwAnimMode == MONMODE_DEATH || pUnit->dwAnimMode == MONMODE_DEAD)
        {
            return 1;
        }
        return 0;
    }

    return 1;
}

//D2Game.0x6FCBD900
// 1.13c D2Game+0xE0A60 (unit in ECX, target in EAX, game on the stack; minion-owner lookups inlined)
// 1.13c walks the target's owner chain while it is a monster (1.10f tested for a player there)
int32_t __fastcall sub_6FCBD900(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pTarget)
{
    D2UnitStrc* pTestUnit = pUnit;
    if (pTestUnit)
    {
        D2UnitStrc* pOwner = nullptr;
        if (pTestUnit->dwUnitType == UNIT_MONSTER)
        {
            pOwner = AIGENERAL_GetMinionOwner(pTestUnit);
        }
        else if (pTestUnit->dwUnitType == UNIT_MISSILE)
        {
            pOwner = SUNIT_GetOwner(pGame, pTestUnit);
        }

        if (pOwner)
        {
            pTestUnit = pOwner;
        }
    }

    D2UnitStrc* i = pTarget;
    while (i && i->dwUnitType == UNIT_MONSTER)
    {
        D2UnitStrc* pOwner = AIGENERAL_GetMinionOwner(i);
        if (!pOwner || pOwner == i)
        {
            break;
        }

        i = pOwner;
    }

    if (pTestUnit == i)
    {
        return 0;
    }

    if (pTestUnit && pTestUnit->dwUnitType == UNIT_PLAYER && i && i->dwUnitType == UNIT_PLAYER)
    {
        return D2_CALL(PLAYERLIST_CheckFlag, pTestUnit, i, 8);
    }

    return STATLIST_AreUnitsAligned(pTestUnit, i) == 0;
}

// not-1.13c: retail has no function for this; the per-type game unit-list index (type -> list, id & 127) is inlined in SUNIT_Add (D2Game+0xE0F40) and its siblings
D2UnitStrc** __fastcall SUNIT_GetUnitList(int32_t nUnitType, D2GameStrc* pGame, int32_t nUnitGUID)
{
    switch (nUnitType)
    {
    case UNIT_PLAYER:
        return &pGame->pUnitList[0][nUnitGUID & 127];

    case UNIT_MONSTER:
        return &pGame->pUnitList[1][nUnitGUID & 127];

    case UNIT_OBJECT:
        return &pGame->pUnitList[2][nUnitGUID & 127];

    case UNIT_MISSILE:
        return &pGame->pUnitList[4][nUnitGUID & 127];

    case UNIT_ITEM:
        return &pGame->pUnitList[3][nUnitGUID & 127];
	default:
		return nullptr;
    }
}

//D2Game.0x6FCBDA40
// 1.13c D2Game+0xE0210 (pet in EAX, owner in ESI, skill in ECX; checked against decompile, no delta)
int32_t __fastcall SUNIT_CanPetBeTargetedBySkill(D2GameStrc* pGame, D2UnitStrc* pOwner, D2UnitStrc* pPet, int32_t nSkillId)
{
    if (!pOwner || !pPet || (pOwner->dwUnitType != UNIT_PLAYER && pPet->dwUnitType != UNIT_MONSTER))
    {
        return 0;
    }

    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord || !(pSkillsTxtRecord->dwFlags[0] & gdwBitMasks[SKILLSFLAGINDEX_TARGETPET]))
    {
        return 0;
    }

    int32_t nOwnerGUID = 0;
    int32_t nOwnerType = 0;
    AIGENERAL_GetOwnerData(pPet, &nOwnerGUID, &nOwnerType);
    if (nOwnerType != UNIT_PLAYER || nOwnerGUID != pOwner->dwUnitId)
    {
        return 0;
    }

    return 1;
}

//D2Game.0x6FCBDAD0
// 1.13c D2Game+0xE0660 (pet in EAX; owner, skill and game on the stack; party lookups via +0xDFA60)
// 1.13c returns 0 when the looked-up unit is missing (1.10f dereferenced it)
int32_t __fastcall SUNIT_CanAllyBeTargetedBySkill(D2GameStrc* pGame, D2UnitStrc* pOwner, D2UnitStrc* pPet, int32_t nSkillId)
{
    if (!pOwner || !pPet || pOwner->dwUnitType != UNIT_PLAYER)
    {
        return 0;
    }
    
    int32_t nUnitType = pPet->dwUnitType;   
    if (nUnitType != UNIT_PLAYER && nUnitType != UNIT_MONSTER)
    {
        return 0;
    }
    
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord || !(pSkillsTxtRecord->dwFlags[0] & gdwBitMasks[SKILLSFLAGINDEX_TARGETALLY]))
    {
        return 0;
    }
    
    int32_t nUnitGUID = pPet->dwUnitId;
    if (nUnitType == UNIT_MONSTER)
    {
        AIGENERAL_GetOwnerData(pPet, &nUnitGUID, &nUnitType);
        if (nUnitGUID == -1 || nUnitType != UNIT_PLAYER)
        {
            return 0;
        }
    }
    
    if (!pOwner->pGame)
    {
        return 0;
    }
    
    const int16_t nPartyId = PARTY_GetPartyIdForUnitOwner(pOwner->pGame, pOwner);
    if (nPartyId == -1)
    {
        return 0;
    }

    D2UnitStrc* pOtherPlayer = SUNIT_GetServerUnit(pGame, nUnitType, nUnitGUID);
    if (!pOtherPlayer || !pOtherPlayer->pGame || pOtherPlayer->dwUnitType != UNIT_PLAYER)
    {
        return 0;
    }
    
    return nPartyId == PARTY_GetPartyIdForUnitOwner(pOtherPlayer->pGame, pOtherPlayer);
}

//D2Game.0x6FCBDC60
// 1.13c D2Game+0xE0570 (unit 1 in ECX, unit 2 in EAX; minion-owner lookups inlined; checked against decompile, no delta)
int32_t __fastcall SUNIT_AreUnitsAligned(D2GameStrc* pGame, D2UnitStrc* pUnit1, D2UnitStrc* pUnit2)
{
    D2_MAYBE_UNUSED(pGame);
    D2UnitStrc* pOwner1 = pUnit1;
    if (pUnit1 && pUnit1->dwUnitType == UNIT_MONSTER)
    {
        D2UnitStrc* pTemp = AIGENERAL_GetMinionOwner(pUnit1);
        if (pTemp)
        {
            pOwner1 = pTemp;
        }
    }

    D2UnitStrc* pOwner2 = pUnit2;
    if (pUnit2 && pUnit2->dwUnitType == UNIT_MONSTER)
    {
        D2UnitStrc* pTemp = AIGENERAL_GetMinionOwner(pUnit2);
        if (pTemp)
        {
            pOwner2 = pTemp;
        }
    }

    if (pOwner1 == pOwner2)
    {
        return 1;
    }

    if (!pOwner1 || pOwner1->dwUnitType != UNIT_PLAYER || !pOwner2 || pOwner2->dwUnitType != UNIT_PLAYER)
    {
        return STATLIST_AreUnitsAligned(pOwner1, pOwner2);
    }

    int16_t nPartyId1 = -1;
    if (pOwner1->pGame)
    {
        nPartyId1 = PARTY_GetPartyIdForUnitOwner(pOwner1->pGame, pOwner1);
    }

    int16_t nPartyId2 = -1;
    if (pOwner2->pGame && pOwner2->dwUnitType == UNIT_PLAYER)
    {
        nPartyId2 = PARTY_GetPartyIdForUnitOwner(pOwner2->pGame, pOwner2);
    }

    return nPartyId1 == nPartyId2 && nPartyId2 != -1;
}

//D2Game.0x6FCBDD30
// 1.13c D2Game+0xE0460
void __fastcall sub_6FCBDD30(D2UnitStrc* pUnit, uint8_t nAlignNew, int32_t a3)
{
    int32_t nAlignment = UNIT_ALIGNMENT_UNASSIGNED;

    if (!pUnit)
    {
        return;
    }

    if (nAlignNew >= UNIT_NUM_ALIGNMENT)
    {
        FOG_DisplayAssert("eAlignNew < NUM_ALIGNMENT", __FILE__, __LINE__);
        exit(-1);
    }

    const int32_t nClassId = pUnit->dwClassId;
    if (nClassId != MONSTER_HYDRA1 && nClassId != MONSTER_HYDRA2 && nClassId != MONSTER_HYDRA3 && nAlignNew == UNIT_ALIGNMENT_EVIL && pUnit->dwFlags >> 31)
    {
        return;
    }

    D2StatListStrc* pStatList = STATLIST_GetStatListFromUnitAndState(pUnit, STATE_ALIGNMENT);
    if (pStatList)
    {
        nAlignment = STATLIST_GetStatValue(pStatList, STAT_ALIGNMENT, 0);
    }
    else
    {
        pStatList = STATLIST_AllocStatList(pUnit->pMemoryPool, 0, 0, pUnit->dwUnitType, pUnit->dwUnitId);
        if (pStatList)
        {
            STATLIST_SetState(pStatList, STATE_ALIGNMENT);
            D2COMMON_10475_PostStatToStatList(pUnit, pStatList, TRUE);
            STATES_ToggleState(pUnit, STATE_ALIGNMENT, TRUE);
        }
        else
        {
            FOG_DisplayWarning("hList", __FILE__, __LINE__);
        }
    }

    STATLIST_SetStatIfListIsValid(pStatList, STAT_ALIGNMENT, nAlignNew, 0);
    STATES_ToggleGfxStateFlag(pUnit, STATE_ALIGNMENT, 1);

    if (pUnit->dwUnitType != UNIT_MONSTER || !pUnit->pGame)
    {
        return;
    }

    if (a3)
    {
        sub_6FC681C0(pUnit->pGame->pMonReg, pUnit, nAlignment, nAlignNew);
    }

    if (nAlignment == nAlignNew)
    {
        return;
    }

    D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
    if (!pRoom)
    {
        return;
    }

    if (nAlignNew == UNIT_ALIGNMENT_GOOD)
    {
        DUNGEON_IncreaseAlliedCountOfRoom(pRoom);
    }
    else if (nAlignment == UNIT_ALIGNMENT_GOOD)
    {
        DUNGEON_DecreaseAlliedCountOfRoom(pRoom);
    }

    if (!MONSTERS_IsDead(pUnit))
    {
        D2Common_10214(pUnit);
    }
}

//D2Game.0x6FCBDE90
// 1.13c D2Game+0xE2240
void __fastcall sub_6FCBDE90(D2UnitStrc* pUnit, int32_t bSetUninterruptable)
{
    STATES_ToggleState(pUnit, STATE_UNINTERRUPTABLE, bSetUninterruptable);

    D2GameStrc* pGame = SUNIT_GetGameFromUnit(pUnit);
    if (!pGame)
    {
        return;
    }

    if (!bSetUninterruptable && STATES_CheckState(pUnit, STATE_DEATH_DELAY))
    {
        STATES_ToggleState(pUnit, STATE_DEATH_DELAY, 0);

        D2UnitStrc* pAttacker = SUNIT_GetLastAttacker(pGame, pUnit);
        if (pAttacker)
        {
            D2DamageStrc damage = {};
            damage.wResultFlags |= DAMAGERESULTFLAG_WILLDIE;
            SUNITDMG_ExecuteMissileDamage(pGame, pAttacker, pUnit, &damage);
        }
        else
        {
            SUNITDMG_KillMonster(pGame, pUnit, nullptr, 1);
        }
    }

    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        D2GAME_EVENTS_Delete_6FC34840(pGame, pUnit, EVENTTYPE_AITHINK, 0);
    }
}

//D2Game.0x6FCBDF90
// 1.13c D2Game+0xE0410 (event callback; checked against decompile, no delta)
int32_t __fastcall sub_6FCBDF90(D2GameStrc* pGame, D2UnitStrc* pPlayer, D2C_EventTypes nEventType, int32_t a4, int32_t a5)
{
	D2_MAYBE_UNUSED(nEventType);
    if (!pPlayer || pPlayer->dwUnitType != UNIT_PLAYER)
    {
        return 0;
    }

    D2PlayerDataStrc* pPlayerData = D2_CALL(UNITS_GetPlayerData, pPlayer);
    if (!pPlayerData)
    {
        return 0;
    }

    pPlayerData->unk0xA8[40] = 0;
    pPlayerData->unk0xA8[41] = 0;
    D2_CALL(sub_6FC349B0, pGame, pPlayer, EVENTTYPE_REMOVESKILLCOOLDOWN, sub_6FCBDF90);
    return 1;
}

//D2Game.0x6FCBDFE0
// 1.13c D2Game+0xE19C0 (nY in EAX, nX in ECX, room in EDX; game, unit, a6, a7 on the stack; checked against disassembly, no delta)
int32_t __fastcall sub_6FCBDFE0(D2GameStrc* pGame, D2UnitStrc* pUnit, D2ActiveRoomStrc* pInputRoom, int32_t nX, int32_t nY, int32_t a6, int32_t a7)
{
    D2_ASSERT(pUnit);
    D2_ASSERT(pUnit->pDynamicPath);

    D2ActiveRoomStrc* pRoom = pInputRoom;
    if (!pRoom)
    {
        pRoom = D2GAME_GetRoom_6FC52070(UNITS_GetRoom(pUnit), nX, nY);
        if (!pRoom)
        {
            return 0;
        }
    }

    D2CoordStrc coords = {};
    coords.nX = nX;
    coords.nY = nY;

    if (!a6)
    {
        pRoom = COLLISION_GetFreeCoordinates(pRoom, &coords, UNITS_GetUnitSizeX(pUnit), 0x1C09u, 0);
    }

    if (!pRoom || !D2Common_10229(pUnit->pDynamicPath, pUnit, pRoom, coords.nX, coords.nY))
    {
        return 0;
    }

    if (pUnit->dwUnitType == UNIT_PLAYER)
    {
        D2ClientStrc* pClient = UNITS_GetPlayerData(pUnit)->pClient;

        D2DrlgCoordsStrc drlgCoords = {};

        DUNGEON_GetRoomCoordinates(pRoom, &drlgCoords);
        D2GAME_PACKETS_SendPacket0x07_6FC3D120(pClient, DUNGEON_GetLevelIdFromRoom(pRoom), (uint16_t)drlgCoords.nTileXPos, (uint16_t)drlgCoords.nTileYPos);

        if (a7)
        {
            UNITROOM_RefreshUnit(pUnit);
            pUnit->dwFlagEx |= 0x800;
        }
        else
        {
            UNITROOM_RefreshUnit(pUnit);
            pUnit->dwFlagEx |= UNITFLAGEX_TELEPORTED;
        }

        sub_6FCBB190(pGame, pUnit, 0);

        D2PlayerDataStrc* pPlayerData = UNITS_GetPlayerData(pUnit);

        pPlayerData->unk0xA8[40] = coords.nX;
        pPlayerData->unk0xA8[41] = coords.nY;
        pPlayerData->unk0xA4 = GetTickCount();

        pPlayerData->unk0xA8[2 * pPlayerData->unk0xA0] = coords.nX;
        pPlayerData->unk0xA8[2 * pPlayerData->unk0xA0 + 1] = coords.nY;

        ++pPlayerData->unk0xA0;
        if (pPlayerData->unk0xA0 >= 20u)
        {
            pPlayerData->unk0xA0 = 0;
        }

        D2GAME_InitTimer_6FC351D0(pGame, pUnit, EVENTTYPE_REMOVESKILLCOOLDOWN, pGame->dwGameFrame + 50, sub_6FCBDF90, 0, 0);
        sub_6FC7E310(pGame, pUnit, coords.nX, coords.nY);
    }
    else
    {
        if (a7)
        {
            UNITROOM_RefreshUnit(pUnit);
            pUnit->dwFlagEx |= 0x800;
        }
        else
        {
            UNITROOM_RefreshUnit(pUnit);
            pUnit->dwFlagEx |= UNITFLAGEX_TELEPORTED;
        }

        sub_6FCBB190(pGame, pUnit, 0);
    }

    return 1;
}

// 1.13c D2Game+0xDFA60
int16_t __fastcall SUNIT_GetPartyId(D2UnitStrc* pPlayer)
{
    if (pPlayer && pPlayer->pGame && pPlayer->dwUnitType == UNIT_PLAYER)
    {
        return PARTY_GetPartyIdForUnitOwner(pPlayer->pGame, pPlayer);
    }

    return -1;
}
