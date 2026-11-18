// Narrow decompile-derived retail item-section save slice.
// Provenance and scope:  This deliberately excludes the character header,
// stats, skills, quests, waypoints, mercenary, corpse and checksum sections.
#include <cstdint>
#include <cstdlib>
#include <Fog.h>
#include <D2Common.h>
#include <D2Inventory.h>
#include <D2BitManip.h>
#include <D2StatList.h>
#include <D2Waypoints.h>
#include <D2Skills.h>
#include <D2States.h>
#include <D2QuestRecord.h>
#include <D2Items.h>
#include <Units/Units.h>
#include <GAME/Clients.h>
#include <GAME/Game.h>
#include <GAME/SCmd.h>
#include <ITEMS/ItemMode.h>
#include <ITEMS/Items.h>
#include <QUESTS/Quests.h>
#include <UNIT/SUnit.h>
#include <PLAYER/PlrSave.h>
#include <PLAYER/PlrSave2.h>
#include <PLAYER/PlrIntro.h>
#include <SKILLS/Skills.h>

#pragma pack(push, 1)
struct D2PlrIntroSaveStrc
{
    uint16_t nHeader;
    uint16_t nLength;
    uint8_t unk0x04[3][8];
    uint8_t unk0x1C[3][8];
};
struct D2WaypointSaveFlagStrc
{
    D2WaypointDataStrc flags;
    uint32_t unk0x18;
    uint32_t unk0x1C;
};
struct D2WaypointSaveDataStrc
{
    uint16_t nHeader;
    int32_t unk0x02;
    uint16_t nRecordSize;
    D2WaypointSaveFlagStrc waypoints[3];
};
#pragma pack(pop)
static_assert(sizeof(D2WaypointSaveDataStrc) == 80);
static_assert(sizeof(D2PlrIntroSaveStrc) == 52);

// 1.13c D2Game+0x54BC0
int32_t __fastcall sub_6FC898F0(D2GameStrc* pGame, D2UnitStrc* pPlayer, D2UnitStrc* pItem, D2UnitStrc* a4)
{
    if (!pItem)
    {
        return 12;
    }

    switch (pItem->dwAnimMode)
    {
    case IMODE_STORED:
    {
        UNITS_ChangeAnimMode(pItem, IMODE_ONCURSOR);
        if (!D2GAME_PlaceItem_6FC44410(__FILE__, __LINE__, pGame, pPlayer, pItem->dwUnitId, CLIENTS_GetUnitX(pItem), CLIENTS_GetUnitY(pItem), 0, 1, 0))
        {
            return 12;
        }
        return 0;
    }
    case IMODE_EQUIP:
    {
        UNITS_ChangeAnimMode(pItem, IMODE_ONCURSOR);
        int32_t nItemGUID = -1;
        if (pItem)
        {
            nItemGUID = pItem->dwUnitId;
        }

        int32_t nUnused = 0;
        if (!sub_6FC45060(pGame, pPlayer, nItemGUID, ITEMS_GetBodyLocation(pItem), 1, &nUnused))
        {
            return 12;
        }
        return 0;
    }
    case IMODE_INBELT:
    {
        UNITS_ChangeAnimMode(pItem, IMODE_ONCURSOR);
        int32_t nUnused = 0;
        if (!sub_6FC48940(pGame, pPlayer, pItem->dwUnitId, CLIENTS_GetUnitX(pItem), 0, &nUnused))
        {
            return 12;
        }
        return 0;
    }
    case IMODE_ONCURSOR:
    {
        UNITS_ChangeAnimMode(pItem, IMODE_ONCURSOR);
        INVENTORY_SetCursorItem(pPlayer->pInventory, pItem);
        if (!sub_6FC4B9D0(pGame, pPlayer, pItem))
        {
            return 12;
        }
        return 0;
    }
    case IMODE_SOCKETED:
    {
        UNITS_ChangeAnimMode(pItem, IMODE_ONCURSOR);

        int32_t nUnused = 0;
        if (!a4 || !D2GAME_ITEMSOCKET_PlaceItem_6FC497E0(pGame, pPlayer, pItem ? pItem->dwUnitId : -1, a4->dwUnitId, &nUnused, 0, 0, 0))
        {
            return 12;
        }

        return 0;
    }
    default:
        return 12;
    }
}

//D2Game.0x6FC89AB0 (#10029)
// 1.13c D2Game+0x54400
int32_t __stdcall D2GAME_10029_SAVE_WriteFileInterface(D2GameStrc* pGame, D2UnitStrc* pPlayer, const char* szName)
{
    return D2GAME_SAVE_WriteFile_6FC8A500(pGame, pPlayer, szName, 0);
}

// 1.13c D2Game+0x53470: only a null item or one flagged 0x8000 in dwFlagEx is skipped.
int32_t __stdcall D2GAME_SAVE_SerializeItem_6FC89AD0(D2UnitStrc* pItem, uint8_t* pBitstream, uint32_t nBitstreamSize, int32_t n0, int32_t a5)
{
    int32_t nBitsLeft = 0;

    if (pItem && !(pItem->dwFlagEx & 0x8000))
    {
        nBitsLeft = ITEMS_SerializeItemToBitstream(pItem, pBitstream, nBitstreamSize, 1, 1, 0);

        if (!nBitsLeft)
        {
            FOG_DisplayAssert("!\"Character has too many items\"", __FILE__, __LINE__);
            exit(-1);
        }
    }

    return nBitsLeft;
}

//D2Game.0x6FC89B50
// 1.13c D2Game+0x537D0: the hand items are serialized once into a scratch buffer before the node walk.
int32_t __stdcall PLRSAVE_WriteItems_6FC89B50(D2InventoryStrc* pInventory, uint8_t* pBitstream, uint32_t nBitstreamSize, int32_t a4, int32_t a5)
{
    if (nBitstreamSize < 2)
    {
        return 0;
    }

    *(WORD*)pBitstream = 0x4D4A; // retail 'MJ' multichar value, bytes "JM" on little endian

    int32_t nItemCount = 0;
    if (pInventory)
    {
        if (INVENTORY_GetCursorItem(pInventory))
        {
            nItemCount = 1;
        }

        for (D2UnitStrc* pItem = INVENTORY_GetFirstItem(pInventory); pItem; pItem = INVENTORY_GetNextItem(pItem))
        {
            if (INVENTORY_UnitIsItem(pItem))
            {
                ++nItemCount;
            }
        }
    }

    if (nBitstreamSize < 4)
    {
        return 2;
    }

    uint8_t* pItemCount = pBitstream + 2;
    *(uint16_t*)pItemCount = nItemCount;

    uint8_t* pData = pItemCount + 2;
    if (!pInventory)
    {
        return pData - pBitstream;
    }

    D2UnitStrc* pWeapon = INVENTORY_GetLeftHandWeapon(pInventory);
    D2UnitStrc* pRightHandItem = INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
    D2UnitStrc* pLeftHandItem = INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
    D2UnitStrc* pCursorItem = INVENTORY_GetCursorItem(pInventory);

    // Retail first serializes the hand items into an 8 KiB scratch buffer (output discarded), failing on
    // a negative size, in the same order as the hand pass below.
    {
        uint8_t scratch[0x2000];
        if (pWeapon)
        {
            D2UnitStrc* pFirst = pRightHandItem == pWeapon ? pRightHandItem : pLeftHandItem;
            D2UnitStrc* pSecond = pRightHandItem == pWeapon ? pLeftHandItem : pRightHandItem;
            if (ITEMS_SerializeItemToBitstream(pFirst, scratch, sizeof(scratch), 1, 1, 0) < 0)
            {
                return -1;
            }

            if (pSecond && ITEMS_SerializeItemToBitstream(pSecond, scratch, sizeof(scratch), 1, 1, 0) < 0)
            {
                return -1;
            }
        }
        else if (pRightHandItem)
        {
            if (ITEMS_SerializeItemToBitstream(pRightHandItem, scratch, sizeof(scratch), 1, 1, 0) < 0)
            {
                return -1;
            }

            if (pLeftHandItem && ITEMS_SerializeItemToBitstream(pLeftHandItem, scratch, sizeof(scratch), 1, 1, 0) < 0)
            {
                return -1;
            }
        }
        else if (pLeftHandItem && ITEMS_SerializeItemToBitstream(pLeftHandItem, scratch, sizeof(scratch), 1, 1, 0) < 0)
        {
            return -1;
        }
    }

    int32_t nSize = 0;
    for (D2UnitStrc* pItem = INVENTORY_GetFirstItem(pInventory); pItem; pItem = INVENTORY_GetNextItem(pItem))
    {
        D2UnitStrc* pCheckedItem = INVENTORY_UnitIsItem(pItem);

        const uint8_t nBodyLoc = ITEMS_GetBodyLocation(pCheckedItem);
        if (INVENTORY_GetItemNodePage(pItem) != 3 || nBodyLoc != BODYLOC_RARM && nBodyLoc != BODYLOC_LARM)
        {
            nSize = D2GAME_SAVE_SerializeItem_6FC89AD0(pCheckedItem, pData, nBitstreamSize + pBitstream - pData, a4, a5);
            if (nSize < 0)
            {
                return -1;
            }

            pData += nSize;

            if (!nSize && (uint16_t)nItemCount > 0u)
            {
                nItemCount += UINT16_MAX;
            }
        }
    }

    if (pWeapon)
    {
        if (pRightHandItem == pWeapon)
        {
            nSize = D2GAME_SAVE_SerializeItem_6FC89AD0(pRightHandItem, pData, nBitstreamSize + pBitstream - pData, a4, a5);
            if (nSize < 0)
            {
                return -1;
            }

            pData += nSize;

            if (!nSize && (uint16_t)nItemCount > 0u)
            {
                nItemCount += UINT16_MAX;
            }

            if (pLeftHandItem)
            {
                nSize = D2GAME_SAVE_SerializeItem_6FC89AD0(pLeftHandItem, pData, nBitstreamSize + pBitstream - pData, a4, a5);
                if (nSize < 0)
                {
                    return -1;
                }

                pData += nSize;

                if (!nSize && (uint16_t)nItemCount > 0u)
                {
                    nItemCount += UINT16_MAX;
                }
            }
        }
        else
        {
            nSize = D2GAME_SAVE_SerializeItem_6FC89AD0(pLeftHandItem, pData, nBitstreamSize + pBitstream - pData, a4, a5);
            if (nSize < 0)
            {
                return -1;
            }

            pData += nSize;

            if (!nSize && (uint16_t)nItemCount > 0u)
            {
                nItemCount += UINT16_MAX;
            }

            if (pRightHandItem)
            {
                nSize = D2GAME_SAVE_SerializeItem_6FC89AD0(pRightHandItem, pData, nBitstreamSize + pBitstream - pData, a4, a5);
                if (nSize < 0)
                {
                    return -1;
                }

                pData += nSize;

                if (!nSize && (uint16_t)nItemCount > 0u)
                {
                    nItemCount += UINT16_MAX;
                }
            }
        }
    }
    else
    {
        if (pRightHandItem)
        {
            nSize = D2GAME_SAVE_SerializeItem_6FC89AD0(pRightHandItem, pData, nBitstreamSize + pBitstream - pData, a4, a5);
            if (nSize < 0)
            {
                return -1;
            }

            pData += nSize;

            if (!nSize && (uint16_t)nItemCount > 0u)
            {
                nItemCount += UINT16_MAX;
            }
        }

        if (pLeftHandItem)
        {
            nSize = D2GAME_SAVE_SerializeItem_6FC89AD0(pLeftHandItem, pData, nBitstreamSize + pBitstream - pData, a4, a5);
            if (nSize < 0)
            {
                return -1;
            }

            pData += nSize;

            if (!nSize && (uint16_t)nItemCount > 0u)
            {
                nItemCount += UINT16_MAX;
            }
        }
    }

    if (pCursorItem && pCursorItem->dwUnitType == UNIT_ITEM)
    {
        nSize = D2GAME_SAVE_SerializeItem_6FC89AD0(pCursorItem, pData, nBitstreamSize + pBitstream - pData, a4, a5);
        if (nSize < 0)
        {
            return -1;
        }

        pData += nSize;

        if (!nSize && (uint16_t)nItemCount > 0u)
        {
            nItemCount += UINT16_MAX;
        }
    }

    *(WORD*)pItemCount = nItemCount;
    return pData - pBitstream;
}

// 1.13c D2Game+0x54810
int32_t __fastcall PLRSAVE_ReadItems_6FC8B8A0(D2GameStrc* pGame, D2UnitStrc* pPlayer, uint8_t* pSection, uint32_t dwVersion, int32_t nSize, int32_t a6, int32_t* pSize)
{
    if (*(uint16_t*)pSection != 0x4D4A)
    {
        return 14;
    }

    const uint16_t nItemCount = *((uint16_t*)pSection + 1);

    uint8_t* pItemBitstream = pSection + 4;
    for (int32_t i = 0; i < nItemCount; ++i)
    {
        const uint32_t nBitstreamSize = pSection - pItemBitstream + nSize;
        D2ItemSaveStrc itemSave = {};
        ITEMS_GetCompactItemDataFromBitstream(pItemBitstream, nBitstreamSize, 1, &itemSave);
        if (itemSave.nClassId == -1)
        {
            return SYSERROR_UNK_14;
        }

        uint32_t nRemainingSize = 0;
        D2UnitStrc* pItem = sub_6FC4EC10(pGame, 0, pItemBitstream, nBitstreamSize, 1, &itemSave, &nRemainingSize, dwVersion);
        if (!pItem && nRemainingSize <= 0)
        {
            return SYSERROR_UNK_14;
        }

        pItemBitstream += nRemainingSize;

        int32_t bRemoveItem = 0;
        if (pItem)
        {
            if (a6)
            {
                bRemoveItem = sub_6FC8BAA0(pGame, pPlayer, pItem, 0);
            }
            else
            {
                bRemoveItem = sub_6FC898F0(pGame, pPlayer, pItem, 0);
            }
        }

        D2UnitStrc* pParentItem = pItem;
        if (bRemoveItem)
        {
            if (pItem)
            {
                SUNIT_RemoveUnit(pGame, pItem);
            }

            pParentItem = nullptr;
        }

        // 1.13c D2Game+0x54810: the socket count is read once (local_24); the loop below overwrites itemSave
        const int32_t nSocketCount = itemSave.nItemFileIndex;
        for (int32_t j = 0; j < nSocketCount; ++j)
        {
            const uint32_t nSocketableBitstreamSize = pSection - pItemBitstream + nSize;
            ITEMS_GetCompactItemDataFromBitstream(pItemBitstream, nSocketableBitstreamSize, 1, &itemSave);

            D2UnitStrc* pSocketable = sub_6FC4EC10(pGame, 0, pItemBitstream, nSocketableBitstreamSize, 1, &itemSave, &nRemainingSize, dwVersion);
            if (!pSocketable && nRemainingSize <= 0)
            {
                return SYSERROR_UNK_14;
            }

            pItemBitstream += nRemainingSize;

            if (pParentItem)
            {
                if (pSocketable)
                {
                    if (a6)
                    {
                        sub_6FC8BAA0(pGame, pPlayer, pSocketable, pParentItem);
                    }
                    else
                    {
                        sub_6FC898F0(pGame, pPlayer, pSocketable, pParentItem);
                    }
                }
            }
            else
            {
                if (pSocketable)
                {
                    SUNIT_RemoveUnit(pGame, pSocketable);
                }
            }
        }

        if (pParentItem && ITEMS_GetItemFlags(pParentItem) & IFLAG_RUNEWORD && !ITEMS_GetRunesTxtRecordFromItem(pParentItem))
        {
            D2GAME_ITEMMODE_Unk_6FC4BC10(pGame, pPlayer, pParentItem);
        }
    }

    if (pSize)
    {
        *pSize = pItemBitstream - pSection;
    }

    return 0;
}

// 1.13c D2Game+0x54A20
int32_t __fastcall sub_6FC8BAA0(D2GameStrc* pGame, D2UnitStrc* pPlayer, D2UnitStrc* pItem, D2UnitStrc* a4)
{
    if (!pItem)
    {
        return SYSERROR_INVENTORY;
    }

    switch (pItem->dwAnimMode)
    {
    case IMODE_STORED:
    {
        UNITS_ChangeAnimMode(pItem, IMODE_ONCURSOR);

        if (D2GAME_PlaceItem_6FC44410(__FILE__, __LINE__, pGame, pPlayer, pItem->dwUnitId, CLIENTS_GetUnitX(pItem), CLIENTS_GetUnitY(pItem), 0, 0, 0))
        {
            return 0;
        }

        return SYSERROR_INVENTORY;
    }
    case IMODE_EQUIP:
    {
        const int32_t nBodyLoc = ITEMS_GetBodyLocation(pItem);
        if (!nBodyLoc || !INVENTORY_PlaceItemInBodyLoc(pPlayer->pInventory, pItem, nBodyLoc))
        {
            return 13;
        }

        int32_t nUnused = 3;
        if (nBodyLoc == 11 || nBodyLoc == 12)
        {
            nUnused = 4;
        }

        if (!INVENTORY_PlaceItemInSocket(pPlayer->pInventory, pItem, nUnused))
        {
            return SYSERROR_INVENTORY;
        }

        ITEMS_SetBodyLocation(pItem, nBodyLoc);

        if (pItem)
        {
            pItem->dwFlags &= 0xFFFFFFFD;
        }

        UNITS_ChangeAnimMode(pItem, 1);
        ITEMS_SetInvPage(pItem, 0xFFu);
        ITEMS_SetItemCMDFlag(pItem, 8, 1);

        if (pItem)
        {
            pItem->dwFlags &= 0xFDFFFFFFu;
        }

        INVENTORY_AddItemToTradeInventory(pPlayer->pInventory, pItem);
        UNITS_RefreshInventory(pPlayer, 1);
        QUESTS_ItemPickedUp(pGame, pPlayer, pItem);
        return 0;
    }
    case IMODE_SOCKETED:
    {
        if (!a4)
        {
            return SYSERROR_INVENTORY;
        }

        UNITS_ChangeAnimMode(pItem, 4);

        int32_t bPlaced = 0;
        if (!D2GAME_ITEMSOCKET_PlaceItem_6FC497E0(pGame, pPlayer, pItem ? pItem->dwUnitId : -1, a4->dwUnitId, &bPlaced, 0, 1u, 0))
        {
            return SYSERROR_INVENTORY;
        }

        return 0;
    }
    default:
        return SYSERROR_INVENTORY;
    }
}

// 1.13c D2Game+0xEBE70: the 'gf' base-stat section; values clamp to the CSV width with no trace calls.
int32_t __fastcall PLRSAVE2_WritePlayerStats(D2GameStrc* pGame, D2UnitStrc* pPlayer, uint8_t** ppSection, const uint8_t* pEnd)
{
    uint8_t* pData = *ppSection + 2;
    if (pData > pEnd)
    {
        return 1;
    }

    *(uint16_t*)*ppSection = 0x6667u;

    D2BitBufferStrc bitBuffer = {};
    BITMANIP_Initialize(&bitBuffer, pData, pEnd - pData);

    if (pPlayer->pStatListEx)
    {
        D2StatStrc statBuffer[512] = {};
        const int32_t nStats = STATLIST_GetBaseStatsData((D2StatListStrc*)pPlayer->pStatListEx, statBuffer, 512);

        for (int32_t i = 0; i < nStats; ++i)
        {
            int32_t nValue = statBuffer[i].nValue;
            const int32_t nLayer = statBuffer[i].nLayer;
            const int32_t nStatId = statBuffer[i].nStat;

            D2ItemStatCostTxt* pItemStatCostTxtRecord = SKILLS_GetItemStatCostTxtRecord(nStatId);
            if (pItemStatCostTxtRecord && pItemStatCostTxtRecord->nCsvBits)
            {
                if (pItemStatCostTxtRecord->nCsvBits < 32)
                {
                    int32_t nMaximum = 0;
                    if (pItemStatCostTxtRecord->dwItemStatFlags & gdwBitMasks[ITEMSTATCOSTFLAGINDEX_CSVSIGNED])
                    {
                        const int32_t nMinimum = -(1 << (pItemStatCostTxtRecord->nCsvBits - 1));
                        nMaximum = (1 << (pItemStatCostTxtRecord->nCsvBits - 1)) - 1;
                        if (nValue < nMinimum)
                        {
                            nValue = nMinimum;
                        }
                    }
                    else
                    {
                        nMaximum = (1 << pItemStatCostTxtRecord->nCsvBits) - 1;
                        if (nValue < 1)
                        {
                            nValue = 0;
                        }
                    }

                    if (nValue >= nMaximum)
                    {
                        nValue = nMaximum;
                    }
                }

                BITMANIP_Write(&bitBuffer, nStatId, 9);
                if (pItemStatCostTxtRecord->nCsvParam)
                {
                    BITMANIP_Write(&bitBuffer, (uint16_t)nLayer, pItemStatCostTxtRecord->nCsvParam);
                }

                BITMANIP_Write(&bitBuffer, nValue, pItemStatCostTxtRecord->nCsvBits);
            }
        }
    }

    BITMANIP_Write(&bitBuffer, 511, 9);

    *ppSection = &pData[BITMANIP_GetSize(&bitBuffer)];

    return 0;
}

// 1.13c D2Game+0xEB910 (checked against decompile)
int32_t __fastcall PLRSAVE2_ReadStatsEx(D2GameStrc* pGame, D2UnitStrc* pUnit, uint8_t** ppSection, uint8_t* pEnd, uint32_t nVersion)
{
    uint8_t* pData = *ppSection + 2;
    if (pData > pEnd)
    {
        return PLRSAVE2ERROR_BAD_STATS;
    }

    if (*(uint16_t*)*ppSection != 0x6667)
    {
        return 18;
    }

    D2BitBufferStrc bitBuffer = {};
    BITMANIP_Initialize(&bitBuffer, pData, pEnd - pData);

    for (int32_t nStatId = BITMANIP_Read(&bitBuffer, 9); nStatId < 511; nStatId = BITMANIP_Read(&bitBuffer, 9))
    {
        D2ItemStatCostTxt* pItemStatCostTxtRecord = SKILLS_GetItemStatCostTxtRecord(nStatId);
        if (!pItemStatCostTxtRecord || !pItemStatCostTxtRecord->nCsvBits)
        {
            FOG_Trace("WARNING: sPLAYERSAVEReadStatsEx, Attempt to set an invalid stat!\n");
            return PLRSAVE2ERROR_BAD_STATS;
        }

        int32_t nLayer = 0;
        if (pItemStatCostTxtRecord->nCsvParam)
        {
            nLayer = BITMANIP_ReadSigned(&bitBuffer, pItemStatCostTxtRecord->nCsvParam);
        }

        int32_t nValue = 0;
        if (pItemStatCostTxtRecord->nCsvBits < 32 && pItemStatCostTxtRecord->dwItemStatFlags & gdwBitMasks[ITEMSTATCOSTFLAGINDEX_CSVSIGNED])
        {
            nValue = BITMANIP_ReadSigned(&bitBuffer, pItemStatCostTxtRecord->nCsvBits);
        }
        else
        {
            nValue = BITMANIP_Read(&bitBuffer, pItemStatCostTxtRecord->nCsvBits);
        }

        STATLIST_SetUnitStat(pUnit, nStatId, nValue, nLayer);
    }

    *ppSection = &pData[BITMANIP_GetSize(&bitBuffer)];
    return 0;
}

// Recovered waypoint writer and reader.
// 1.13c D2Game+0xEB700
int32_t __fastcall PLRSAVE2_WriteWaypointData(D2GameStrc* pGame, D2UnitStrc* pUnit, uint8_t** ppSection, const uint8_t* pEnd)
{
    if (*ppSection + sizeof(D2WaypointSaveDataStrc) > pEnd)
    {
        return 1;
    }

    D2PlayerDataStrc* pPlayerData = UNITS_GetPlayerData(pUnit);
    if (!pPlayerData)
    {
        return 2;
    }

    D2WaypointSaveDataStrc waypointSaveData = {};
    // The marker is the two bytes 'W','S', so the little-endian u16 is 0x5357 -- NOT D2SMAGIC_WPINFO
    // (0x5753), which is the char-order number and emits "SW" when stored directly. D2MOO's own
    // source is inconsistent here: its constants table says 0x5753 while its writer uses the
    // literal 'SW' (= 0x5357 to a C compiler, which is the correct one). Confirmed against two
    // independent readers of real save files: d2s-format ("starts with 2 chars \"WS\"") and d2s-ts
    // ("waypoint header identifier = 0x57, 0x53"). The quest section has the same trap and is
    // already written byte-reversed (0x216F6F57) for the same reason.
    waypointSaveData.nHeader = 0x5357;
    waypointSaveData.unk0x02 = 1;
    waypointSaveData.nRecordSize = sizeof(D2WaypointSaveDataStrc);

    for (int32_t i = 0; i < 3; ++i)
    {
        WAYPOINTS_ValidateAndCopyWaypointData(pPlayerData->pWaypointData[i], &waypointSaveData.waypoints[i].flags);
    }

    memcpy(*ppSection, &waypointSaveData, sizeof(D2WaypointSaveDataStrc));
    *ppSection += sizeof(D2WaypointSaveDataStrc);

    return 0;
}


// 1.13c D2Game+0xEB4E0 (checked against decompile)
int32_t __fastcall PLRSAVE2_ReadWaypointData(D2GameStrc* pGame, D2UnitStrc* pUnit, uint8_t** ppSection, uint8_t* pEnd, int32_t nUnused)
{
    if (*ppSection + sizeof(D2WaypointSaveDataStrc) > pEnd)
    {
        return PLRSAVE2ERROR_BAD_WAYPOINT;
    }

    D2WaypointSaveDataStrc* pWaypointSaveData = (D2WaypointSaveDataStrc*)*ppSection;
    if (pWaypointSaveData->nHeader != 0x5357)
    {
        return PLRSAVE2ERROR_BAD_WAYPOINT;
    }

    D2PlayerDataStrc* pPlayerData = UNITS_GetPlayerData(pUnit);
    for (int32_t i = 0; i < 3; ++i)
    {
        D2WaypointDataStrc* pSavedData = &pWaypointSaveData->waypoints[i].flags;

        if (pSavedData->nFlags[0] != 0x102 && pSavedData->nFlags[0] != 0x101 && pSavedData->nFlags[0])
        {
            return PLRSAVE2ERROR_BAD_WAYPOINT;
        }

        WAYPOINTS_CopyAndValidateWaypointData(pPlayerData->pWaypointData[i], pSavedData);
    }

    *ppSection += sizeof(D2WaypointSaveDataStrc);
    return 0;
}

// Recovered current-version player skill reader.
// 1.13c D2Game+0x53D40
int32_t __fastcall PLRSAVE2_ReadSkills(D2GameStrc* pGame, D2UnitStrc* pPlayer, uint8_t** ppSection, uint8_t* pEnd, uint32_t nVersion, int32_t nSkills)
{
    uint8_t* pData = *ppSection + 2;
    if (pData > pEnd)
    {
        return PLRSAVE2ERROR_BAD_SKILLS;
    }

    if (*(uint16_t*)*ppSection != 0x6669)
    {
        return PLRSAVE2ERROR_BAD_SKILLS;
    }

    D2ClientStrc* pClient = SUNIT_GetClientFromPlayer(pPlayer, __FILE__, __LINE__);
    if (!pClient)
    {
        return PLRSAVE2ERROR_BAD_SKILLS;
    }

    const uint8_t nClassId = pClient->nClassId;
    for (int32_t i = 0; i < nSkills; ++i)
    {
        const int32_t nSkillId = SKILLS_GetClassSkillId(nClassId, i);
        if (i > SKILLS_GetPlayerSkillCount(nClassId))
        {
            break;
        }

        if (pData[i])
        {
            D2GAME_SetSkills_6FD14C60(pPlayer, nSkillId, pData[i], 1);
        }
    }

    // Retail emits packet 0x94 here. The headless session has no network transport; this packet
    // mirrors the skill list and does not participate in the recovered state mutation.
    sub_6FD14C30(pPlayer);

    *ppSection = &pData[nSkills];
    return 0;
}

// Recovered native quest record section extracted from PLRSAVE2_CreateSaveFile/ProcessSaveFile.
// 1.13c D2Game+0xEB7B0
int32_t __fastcall D2WEB_PLRSAVE2_WriteQuestSection(D2UnitStrc* pPlayer, uint8_t** ppSection, const uint8_t* pEnd)
{
    if (*ppSection + 298 > pEnd) return 1;
    D2PlayerDataStrc* pPlayerData = UNITS_GetPlayerData(pPlayer);
    if (!pPlayerData) return 2;
    uint8_t buffer[298] = {};
    *(uint32_t*)buffer = 0x216F6F57u;
    *(uint32_t*)&buffer[4] = 6;
    *(uint16_t*)&buffer[8] = 298;
    for (int32_t i = 0; i < 3; ++i)
        QUESTRECORD_CopyRecordToBuffer(pPlayerData->pQuestData[i], &buffer[10 + 96 * i], 96, 1);
    memcpy(*ppSection, buffer, sizeof buffer);
    *ppSection += sizeof buffer;
    return 0;
}

// 1.13c D2Game+0xEB560: no player-data check (retail reads the player data unguarded).
int32_t __fastcall D2WEB_PLRSAVE2_ReadQuestSection(D2UnitStrc* pPlayer, uint8_t** ppSection, const uint8_t* pEnd)
{
    if (*ppSection + 298 > pEnd || *(uint32_t*)*ppSection != 0x216F6F57u || *(uint32_t*)(*ppSection + 4) != 6)
        return PLRSAVE2ERROR_BAD_QUESTRECORD;
    D2PlayerDataStrc* pPlayerData = UNITS_GetPlayerData(pPlayer);
    uint8_t* record = *ppSection + 10;
    for (int32_t i = 0; i < 3; ++i, record += 96)
        QUESTRECORD_CopyBufferToRecord(pPlayerData->pQuestData[i], record, 96, 1);
    *ppSection += 298;
    return 0;
}

// Recovered player-intro writer and the paired inline reader from PLRSAVE2_ProcessSaveFile.
// 1.13c D2Game+0xEB610 (checked against disassembly; retail inlines the copy helpers)
int32_t __fastcall PLRSAVE2_WritePlrIntroSection(D2GameStrc* pGame, D2UnitStrc* pUnit, uint8_t** ppSection, const uint8_t* pEnd)
{
    if (*ppSection + sizeof(D2PlrIntroSaveStrc) > pEnd) return 1;
    D2PlayerDataStrc* pPlayerData = UNITS_GetPlayerData(pUnit);
    if (!pPlayerData) return 2;
    D2PlrIntroSaveStrc save = {};
    save.nHeader = 0x7701u;
    save.nLength = sizeof(D2PlrIntroSaveStrc);
    for (int32_t i = 0; i < 3; ++i) {
        memcpy(save.unk0x04[i], pPlayerData->pPlayerIntro[i]->pQuestIntroFlags->pBuffer, 8);
        memcpy(save.unk0x1C[i], pPlayerData->pPlayerIntro[i]->pNpcIntroFlags->pBuffer, 8);
    }
    memcpy(*ppSection, &save, sizeof save);
    *ppSection += sizeof save;
    return 0;
}

// 1.13c D2Game+0xEB440: the 0x7701 section copied straight into the six intro bit buffers (the copy
// helpers inlined), with no player-data check.
int32_t __fastcall D2WEB_PLRSAVE2_ReadPlrIntroSection(D2UnitStrc* pPlayer, uint8_t** ppSection, const uint8_t* pEnd)
{
    if (*ppSection + sizeof(D2PlrIntroSaveStrc) > pEnd) return PLRSAVE2ERROR_BAD_PLAYERINTRO;
    D2PlrIntroSaveStrc* save = (D2PlrIntroSaveStrc*)*ppSection;
    if (save->nHeader != 0x7701u) return PLRSAVE2ERROR_BAD_PLAYERINTRO;
    D2PlayerDataStrc* pPlayerData = UNITS_GetPlayerData(pPlayer);
    for (int32_t i = 0; i < 3; ++i) {
        memcpy(pPlayerData->pPlayerIntro[i]->pQuestIntroFlags->pBuffer, save->unk0x04[i], 8);
        memcpy(pPlayerData->pPlayerIntro[i]->pNpcIntroFlags->pBuffer, save->unk0x1C[i], 8);
    }
    *ppSection += sizeof *save;
    return 0;
}

// not-1.13c: retail passes pUnit->pInventory straight to its item writer D2Game+0x537D0 (from 0x53B40 via
// 0xEC0E0, and from 0xEC1E0 and 0x70FB0); this wrapper stays because the trade code calls it.
int32_t __fastcall sub_6FC8A0F0(D2GameStrc* pGame, D2UnitStrc* pUnit, uint8_t* pSection, int32_t nSize, int32_t a5, int32_t a6)
{
    return PLRSAVE_WriteItems_6FC89B50(pUnit->pInventory, pSection, nSize, a5, a6);
}

// not-1.13c: retail's item-section dispatch D2Game+0x55050 also sends version 0x47 to the legacy reader
// D2Game+0x54D90, which is not ported; versions 0x48..0x60 go to 0x54810 as here, anything else is 1.
int32_t __fastcall sub_6FC8BC70(D2GameStrc* pGame, D2UnitStrc* pUnit, uint8_t* pSection, uint32_t dwVersion, int32_t nSize, int32_t a6, int32_t* pSize)
{
    if (dwVersion >= 72 && dwVersion <= 96) return PLRSAVE_ReadItems_6FC8B8A0(pGame, pUnit, pSection, dwVersion, nSize, a6, pSize);
    return 1;
}
// 1.13c D2Game+0xEC260: of the corpses holding items, the one whose non-gold items sum to the largest
// transaction cost (count starts at 1, a later corpse wins a tie) is the one written.
int32_t __fastcall PLRSAVE2_WriteCorpsesSection(D2GameStrc* pGame, D2UnitStrc* pPlayer, uint8_t** ppSection, const uint8_t* pEnd, int32_t a5)
{
    if (!pPlayer->pInventory)
    {
        return 2;
    }

    uint32_t nBest = 0;
    D2UnitStrc* pUnit = nullptr;
    for (D2CorpseStrc* pCorpse = INVENTORY_GetFirstCorpse(pPlayer->pInventory); pCorpse; pCorpse = INVENTORY_GetNextCorpse(pCorpse))
    {
        D2UnitStrc* pCorpseUnit = SUNIT_GetServerUnit(pGame, UNIT_PLAYER, INVENTORY_GetUnitGUIDFromCorpse(pCorpse));
        if (pCorpseUnit && pCorpseUnit->pInventory && INVENTORY_GetItemCount(pCorpseUnit->pInventory) > 0)
        {
            uint32_t nValue = 1;
            for (D2UnitStrc* pNode = INVENTORY_GetFirstItem(pCorpseUnit->pInventory); pNode; pNode = INVENTORY_GetNextItem(pNode))
            {
                D2UnitStrc* pItem = INVENTORY_UnitIsItem(pNode);
                if (!ITEMS_CheckItemTypeId(pItem, ITEMTYPE_GOLD))
                {
                    nValue += ITEMS_GetTransactionCost(pPlayer, pItem, (D2C_Difficulties)3, UNITS_GetPlayerData(pPlayer)->pQuestData[pGame->nDifficulty], 0x201, (D2C_TransactionTypes)0);
                }
            }

            if (nValue >= nBest)
            {
                nBest = nValue;
                pUnit = pCorpseUnit;
            }
        }
    }

    uint8_t* pSection = *ppSection;
    if (pSection + 4 > pEnd)
    {
        return 1;
    }

    *(uint16_t*)pSection = 0x4D4A;
    pSection += 2;

    *(uint16_t*)pSection = pUnit ? 1 : 0;
    pSection += 2;

    if (!pUnit)
    {
        *ppSection = pSection;
        return 0;
    }

    if (pSection + 12 > pEnd)
    {
        return 1;
    }

    *(uint32_t*)pSection = 0; // retail leaves this dword as stack residue
    *((uint32_t*)pSection + 1) = CLIENTS_GetUnitX(pUnit);
    *((uint32_t*)pSection + 2) = CLIENTS_GetUnitY(pUnit);

    pSection += 12;

    const int32_t nSectionSize = sub_6FC8A0F0(pGame, pUnit, pSection, pEnd - pSection, 0, a5);
    if (nSectionSize < 0)
    {
        return 4;
    }

    *ppSection = &pSection[nSectionSize];
    return 0;
}

// 1.13c D2Game+0xECF30: the client is read inline (no player or not a player -> 101), and a class past
// the seven player classes fails the allocation (106) before the body unit is created.
int32_t __fastcall PLRSAVE2_ReadCorpses(D2GameStrc* pGame, D2UnitStrc* pPlayer, uint8_t** ppSection, uint8_t* pEnd, uint32_t nVersion, int32_t* a6)
{
    D2ClientStrc* pClient = nullptr;
    if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER)
    {
        pClient = UNITS_GetPlayerData(pPlayer)->pClient;
    }

    if (!pClient)
    {
        if (a6)
        {
            *a6 = 101;
        }

        return PLRSAVE2ERROR_BAD_CORPSES;
    }

    if (*ppSection + 4 > pEnd)
    {
        if (a6)
        {
            *a6 = 102;
        }

        return PLRSAVE2ERROR_BAD_CORPSES;
    }

    if (*(uint16_t*)*ppSection != 0x4D4A)
    {
        if (a6)
        {
            *a6 = 103;
        }

        return PLRSAVE2ERROR_BAD_CORPSES;
    }

    const uint16_t nCount = *((uint16_t*)*ppSection + 1);
    if (nCount > 1)
    {
        if (a6)
        {
            *a6 = 104;
        }

        return PLRSAVE2ERROR_BAD_CORPSES;
    }

    uint8_t* pDataStart = *ppSection + 4;
    for (int32_t i = 0; i < (int32_t)nCount; ++i)
    {
        uint8_t* pData = pDataStart + 12;
        if (pData > pEnd)
        {
            if (a6)
            {
                *a6 = 105;
            }

            return PLRSAVE2ERROR_BAD_CORPSES;
        }

        D2UnitStrc* pPlayerBody = nullptr;
        if (pPlayer->dwClassId < 7)
        {
            pPlayerBody = SUNIT_AllocUnitData(UNIT_PLAYER, pPlayer->dwClassId, 0, 0, pGame, nullptr, 1, PLRMODE_DEAD, 0);
        }

        if (!pPlayerBody)
        {
            if (a6)
            {
                *a6 = 106;
            }

            return PLRSAVE2ERROR_BAD_CORPSES;
        }

        pPlayerBody->dwFlags &= 0xFFFFFFFBu;

        SUNIT_InitClientInPlayerData(pPlayerBody, pClient);
        UNITS_SetNameInPlayerData(pPlayerBody, CLIENTS_GetName(pClient));
        SUNIT_InitSeed(pPlayerBody, &pGame->pGameSeed);
        STATES_ToggleState(pPlayerBody, STATE_PLAYERBODY, 1);

        pPlayerBody->dwFlags &= 0xFFFFFFEFu;

        INVENTORY_SetOwnerId(pPlayerBody->pInventory, pPlayer->dwUnitId);

        int32_t nLength = 0;
        const int32_t nErrorCode = sub_6FC8BC70(pGame, pPlayerBody, pData, nVersion, pEnd - pData, 0, &nLength);
        if (nErrorCode)
        {
            if (a6)
            {
                *a6 = nErrorCode;
            }

            return PLRSAVE2ERROR_BAD_CORPSES;
        }

        pDataStart = &pData[nLength];

        INVENTORY_CreateCorpseForPlayer(pPlayer->pInventory, pPlayerBody->dwUnitId, 0, 1);
        D2GAME_SendPacket0x8E_CorpseAssign_ToAllPlayers_6FC3F9B0(pGame, pPlayer->dwUnitId, pPlayerBody->dwUnitId, 1u);
    }

    *ppSection = pDataStart;
    return 0;
}

// 1.13c D2Game+0x52A20: legacy hireling experience factor by id and level band (read from the decompile; matches the 1.10f body).
int32_t __fastcall sub_6FC8BEE0(int16_t nHirelingId, int32_t nLevel)
{
    switch (nHirelingId)
    {
    case 0:
    case 1:
        if (nLevel < 49)
        {
            return (((nLevel < 25) - 1) & 100) + 100;
        }
        return 400;
    case 2:
    case 3:
        return (((nLevel < 49) - 1) & 220) + 220;
    case 4:
    case 5:
    case 12:
    case 13:
    case 14:
    case 21:
    case 23:
        return 480;
    case 6:
    case 7:
    case 8:
        if (nLevel < 55)
        {
            return (((nLevel < 31) - 1) & 105) + 105;
        }
        return 420;
    case 9:
    case 10:
    case 11:
        return (((nLevel < 55) - 1) & 230) + 230;
    case 15:
    case 17:
        if (nLevel < 61)
        {
            return (((nLevel < 37) - 1) & 110) + 110;
        }
        return 440;
    case 16:
        if (nLevel < 61)
        {
            return (((nLevel < 37) - 1) & 115) + 115;
        }
        return 260;
    case 18:
    case 20:
        return (((nLevel < 61) - 1) & 230) + 230;
    case 19:
        return (((nLevel < 61) - 1) & 240) + 240;
    case 22:
        return 500;
    case 24:
    case 25:
        if (nLevel < 75)
        {
            return (((nLevel < 42) - 1) & 220) + 220;
        }
        return 880;
    case 26:
    case 27:
        return (((nLevel < 75) - 1) & 460) + 460;
    default:
        return 960;
    }
}
