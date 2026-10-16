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
int32_t __stdcall D2GAME_10029_SAVE_WriteFileInterface(D2GameStrc* pGame, D2UnitStrc* pPlayer, const char* szName)
{
    return D2GAME_SAVE_WriteFile_6FC8A500(pGame, pPlayer, szName, 0);
}
int32_t __stdcall D2GAME_SAVE_SerializeItem_6FC89AD0(D2UnitStrc* pItem, uint8_t* pBitstream, uint32_t nBitstreamSize, int32_t n0, int32_t a5)
{
    int32_t nBitsLeft = 0;

    if (pItem && !(pItem->dwFlagEx & 0x8000)/* && (n0 || !a5 || D2GAME_Return1_6FC41900())*/)
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

    D2UnitStrc* pWeapon = INVENTORY_GetLeftHandWeapon(pInventory);
    D2UnitStrc* pRightHandItem = INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
    D2UnitStrc* pLeftHandItem = INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
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

    D2UnitStrc* pCursorItem = INVENTORY_GetCursorItem(pInventory);
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

        for (int32_t j = 0; j < itemSave.nItemFileIndex; ++j)
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

// Recovered current-version gf player base-stat section.
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
                    if (pItemStatCostTxtRecord->dwItemStatFlags & gdwBitMasks[ITEMSTATCOSTFLAGINDEX_CSVSIGNED])
                    {
                        const int32_t nMinimum = -(1 << (pItemStatCostTxtRecord->nCsvBits - 1));
                        int32_t nMaximum = 1 << (pItemStatCostTxtRecord->nCsvBits - 1);
                        if (nValue <= nMinimum || nValue >= nMaximum)
                        {
                            FOG_Trace("Save error: stat:%d  value:%d", nStatId, nValue);
                        }

                        nMaximum -= 1;

                        if (nValue < nMinimum)
                        {
                            nValue = nMinimum;
                        }

                        if (nValue > nMaximum)
                        {
                            nValue = nMaximum;
                        }
                    }
                    else
                    {
                        int32_t nMaximum = 1 << pItemStatCostTxtRecord->nCsvBits;
                        if (nValue < 0 || nValue >= nMaximum)
                        {
                            FOG_Trace("Save error: stat:%d  value:%d", nStatId, nValue);
                        }

                        nMaximum -= 1;

                        if (nValue < 0)
                        {
                            nValue = 0;
                        }

                        if (nValue >= nMaximum)
                        {
                            nValue = nMaximum;
                        }
                    }
                }

                BITMANIP_Write(&bitBuffer, nStatId, 9);
                if (pItemStatCostTxtRecord->nCsvParam)
                {
                    if (nLayer >= 1 << pItemStatCostTxtRecord->nCsvParam)
                    {
                        FOG_Trace("Save error: stat:%d  param:%d", nStatId, nLayer);
                    }

                    BITMANIP_Write(&bitBuffer, nLayer, pItemStatCostTxtRecord->nCsvParam);
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

int32_t __fastcall D2WEB_PLRSAVE2_ReadQuestSection(D2UnitStrc* pPlayer, uint8_t** ppSection, const uint8_t* pEnd)
{
    if (*ppSection + 298 > pEnd || *(uint32_t*)*ppSection != 0x216F6F57u || *(uint32_t*)(*ppSection + 4) != 6)
        return PLRSAVE2ERROR_BAD_QUESTRECORD;
    D2PlayerDataStrc* pPlayerData = UNITS_GetPlayerData(pPlayer);
    if (!pPlayerData) return PLRSAVE2ERROR_BAD_QUESTRECORD;
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
        PLRINTRO_CopyQuestIntroFlagsToBuffer(pPlayerData->pPlayerIntro[i], save.unk0x04[i]);
        PLRINTRO_CopyNpcIntroFlagsToBuffer(pPlayerData->pPlayerIntro[i], save.unk0x1C[i]);
    }
    memcpy(*ppSection, &save, sizeof save);
    *ppSection += sizeof save;
    return 0;
}

int32_t __fastcall D2WEB_PLRSAVE2_ReadPlrIntroSection(D2UnitStrc* pPlayer, uint8_t** ppSection, const uint8_t* pEnd)
{
    if (*ppSection + sizeof(D2PlrIntroSaveStrc) > pEnd) return PLRSAVE2ERROR_BAD_PLAYERINTRO;
    D2PlrIntroSaveStrc* save = (D2PlrIntroSaveStrc*)*ppSection;
    if (save->nHeader != 0x7701u) return PLRSAVE2ERROR_BAD_PLAYERINTRO;
    D2PlayerDataStrc* pPlayerData = UNITS_GetPlayerData(pPlayer);
    if (!pPlayerData) return PLRSAVE2ERROR_BAD_PLAYERINTRO;
    for (int32_t i = 0; i < 3; ++i) {
        PLRINTRO_CopyBufferToQuestIntroFlags(pPlayerData->pPlayerIntro[i], save->unk0x04[i]);
        PLRINTRO_CopyBufferToNpcIntroFlags(pPlayerData->pPlayerIntro[i], save->unk0x1C[i]);
    }
    *ppSection += sizeof *save;
    return 0;
}

int32_t __fastcall sub_6FC8A0F0(D2GameStrc* pGame, D2UnitStrc* pUnit, uint8_t* pSection, int32_t nSize, int32_t a5, int32_t a6)
{
    return PLRSAVE_WriteItems_6FC89B50(pUnit->pInventory, pSection, nSize, a5, a6);
}

int32_t __fastcall sub_6FC8BC70(D2GameStrc* pGame, D2UnitStrc* pUnit, uint8_t* pSection, uint32_t dwVersion, int32_t nSize, int32_t a6, int32_t* pSize)
{
    if (dwVersion >= 72 && dwVersion <= 96) return PLRSAVE_ReadItems_6FC8B8A0(pGame, pUnit, pSection, dwVersion, nSize, a6, pSize);
    return 1;
}
int32_t __fastcall PLRSAVE2_WriteCorpsesSection(D2GameStrc* pGame, D2UnitStrc* pPlayer, uint8_t** ppSection, const uint8_t* pEnd, int32_t a5)
{
    if (!pPlayer->pInventory)
    {
        return 2;
    }

    uint16_t bHasCorpseWithItems = 0;
    D2UnitStrc* pUnit = nullptr;
    for (D2CorpseStrc* pCorpse = INVENTORY_GetFirstCorpse(pPlayer->pInventory); pCorpse; pCorpse = INVENTORY_GetNextCorpse(pCorpse))
    {
        pUnit = SUNIT_GetServerUnit(pGame, UNIT_PLAYER, INVENTORY_GetUnitGUIDFromCorpse(pCorpse));
        if (pUnit && pUnit->pInventory && INVENTORY_GetItemCount(pUnit->pInventory))
        {
            bHasCorpseWithItems = 1;
            break;
        }
    }

    uint8_t* pSection = *ppSection;
    if (pSection + 4 > pEnd)
    {
        return 1;
    }

    *(uint16_t*)pSection = 0x4D4A;
    pSection += 2;

    *(uint16_t*)pSection = bHasCorpseWithItems;
    pSection += 2;

    if (!pUnit || !bHasCorpseWithItems)
    {
        *ppSection = pSection;
        return 0;
    }

    if (pSection + 12 > pEnd)
    {
        return 1;
    }

    //*(uint32_t*)pSection = v23; // TODO: Maybe Corpse Id??? In IDA it's undefined
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

int32_t __fastcall PLRSAVE2_ReadCorpses(D2GameStrc* pGame, D2UnitStrc* pPlayer, uint8_t** ppSection, uint8_t* pEnd, uint32_t nVersion, int32_t* a6)
{
    D2ClientStrc* pClient = SUNIT_GetClientFromPlayer(pPlayer, __FILE__, __LINE__);

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

        D2UnitStrc* pPlayerBody = SUNIT_AllocUnitData(UNIT_PLAYER, pPlayer ? pPlayer->dwClassId : -1, 0, 0, pGame, nullptr, 1, PLRMODE_DEAD, 0);
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

        INVENTORY_SetOwnerId(pPlayerBody->pInventory, pPlayer ? pPlayer->dwUnitId : -1);

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
