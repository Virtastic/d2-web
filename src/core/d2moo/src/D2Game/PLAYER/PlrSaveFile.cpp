// The .d2s file: the 765-byte prefix and the section assembly that turns the already-recovered
// per-section writers into a character file retail could load.
//
// Ported from D2MOO `D2Game/src/PLAYER/PlrSave2.cpp` (PLRSAVE2_WriteSaveHeader @0x6FC8CD80,
// PLRSAVE2_CreateSaveFile, PLRSAVE2_GetItemIndex, PLRSAVE2_WriteMercData) and
// `D2Game/src/PLAYER/PlrSave.cpp` (D2GAME_SAVE_CalculateChecksum @0x6FC8A140). Every section writer
// this calls was already vendored in PlrSaveItems.cpp; what was missing was the header and the
// order they go in.
//
// Cross-check worth recording: D2MOO's D2SaveHeaderStrc is 335 bytes, and the community's
// d2s-format description calls the file's prefix "765 bytes". Those agree exactly --
// 335 (header) + 298 (quests) + 81 (waypoints) + 51 (NPC intro) = 765 -- which is two independent
// descriptions of the same layout landing on the same number.
#include <D2Common.h>
#include <D2DataTbls.h>
#include <D2StatList.h>
#include <D2Inventory.h>
#include <D2Skills.h>
#include <D2Waypoints.h>
#include <D2QuestRecord.h>
#include <Units/Units.h>
#include <GAME/Game.h>
#include <GAME/Clients.h>
#include "SKILLS/Skills.h"
#include <PLAYER/PlrSave.h>
#include <PLAYER/PlrSave2.h>
#include <PLAYER/Player.h>
#include <PLAYER/PlayerPets.h>
#include <UNIT/SUnit.h>
#include <cstring>
#include <ctime>
#include <climits>

//D2Game.0x6FC8A140
int32_t __fastcall D2GAME_SAVE_CalculateChecksum_6FC8A140(D2SaveHeaderStrc* pSaveHeader, int32_t nSize)
{
    // Retail zeroes the checksum field itself while summing, then restores it. The sum is a 32-bit
    // rotate-left-through-carry: the sign bit that falls off the top is added back in at the bottom.
    const uint32_t nSavedChecksum = pSaveHeader->dwChecksum;
    pSaveHeader->dwChecksum = 0;

    int32_t nChecksum = 0;
    const uint8_t* pData = (const uint8_t*)pSaveHeader;
    for (int32_t i = 0; i < nSize; ++i)
    {
        int32_t nByte = pData[i];
        if (nChecksum < 0)
        {
            ++nByte;
        }
        nChecksum = (int32_t)(((uint32_t)nChecksum << 1) + (uint32_t)nByte);
    }

    pSaveHeader->dwChecksum = nSavedChecksum;
    return nChecksum;
}

//D2Game.0x6FC8CC50
int16_t __fastcall PLRSAVE2_GetItemIndex(D2UnitStrc* pPlayer, int32_t nGUID)
{
    if (nGUID == -1 || !pPlayer || !pPlayer->pInventory)
    {
        return 0;
    }

    int16_t nIdx = 1;
    D2UnitStrc* pItem = INVENTORY_GetFirstItem(pPlayer->pInventory);
    while (pItem)
    {
        D2UnitStrc* pCheckedItem = INVENTORY_UnitIsItem(pItem);
        D2UnitStrc* pNextItem = INVENTORY_GetNextItem(pItem);
        if ((pCheckedItem ? (int32_t)pCheckedItem->dwUnitId : -1) == nGUID)
        {
            return nIdx;
        }

        ++nIdx;
        pItem = pNextItem;
    }

    return 0;
}

//D2Game.0x6FC8CCD0
void __fastcall PLRSAVE2_WriteMercData(D2GameStrc* pGame, D2UnitStrc* pPlayer, D2SaveHeaderStrc* pMercData)
{
    D2UnitStrc* pHireling = sub_6FC7E8B0(pGame, pPlayer, PETTYPE_HIREABLE, 1);
    if (!pHireling)
    {
        return;
    }

    D2PetInfoStrc* pPetInfo = PLAYERPETS_GetPetInfoFromPetGUID(pPlayer, pHireling->dwUnitId);
    if (!pPetInfo)
    {
        return;
    }
    const int32_t nLevel = STATLIST_UnitGetStatValue(pHireling, STAT_LEVEL, 0);

    D2HirelingTxt* pHirelingTxtRecord = DATATBLS_GetHirelingTxtRecordFromIdAndLevel(pGame->bExpansion, pPetInfo->nHirelingId, nLevel);
    if (!pHirelingTxtRecord)
    {
        return;
    }

    pMercData->MercSaveData.nSeed = pPetInfo->nSeed;
    pMercData->MercSaveData.wName = pPetInfo->wName;
    pMercData->MercSaveData.nHirelingId = pPetInfo->nHirelingId;
    pMercData->MercSaveData.nExperience = STATLIST_UnitGetStatValue(pHireling, STAT_EXPERIENCE, 0);
    pMercData->MercSaveData.nFlags = 0;
    // Retail stores the name as an offset into the hireling's name range, not the absolute index.
    pMercData->MercSaveData.wName -= pHirelingTxtRecord->wNameFirst;

    if (SUNIT_IsDead(pHireling))
    {
        pMercData->MercSaveData.nFlags |= UNITFLAG_ISDEAD;
    }
}

//D2Game.0x6FC8CD80
int32_t __fastcall PLRSAVE2_WriteSaveHeader(D2GameStrc* pGame, D2UnitStrc* pPlayer, uint8_t** ppSection, const uint8_t* pEnd)
{
    D2ClientStrc* pClient = SUNIT_GetClientFromPlayer(pPlayer, __FILE__, __LINE__);
    if (!pClient)
    {
        return 2;
    }

    const char* szName = CLIENTS_GetName(pClient);
    if (!szName || szName[0] == '\0')
    {
        return 2;
    }

    // The name must fit in 16 bytes INCLUDING its terminator; retail refuses anything longer
    // rather than truncating it.
    int32_t i = 0;
    while (i < 16 && szName[i] != '\0')
    {
        ++i;
    }
    if (i == 16)
    {
        return 2;
    }

    if (*ppSection + sizeof(D2SaveHeaderStrc) > pEnd)
    {
        return 1;
    }

    D2SaveHeaderStrc saveHeader = {};
    memset(&saveHeader, 0, sizeof(D2SaveHeaderStrc));

    saveHeader.dwHeaderMagic = D2SMAGIC_HEADER;
    saveHeader.dwVersion = 96;
    memcpy(saveHeader.szName, szName, (size_t)i);
    saveHeader.dwSaveFlags = CLIENTS_GetFlags(pClient);
    if (pGame->bExpansion)
    {
        saveHeader.tPackedSaveFlags.bExpansion = true;
    }
    if (pGame->dwGameType)
    {
        saveHeader.tPackedSaveFlags.bLadder = true;
    }

    saveHeader.nClass = (uint8_t)CLIENTS_GetClassId(pClient);
    saveHeader.nLevel = (uint8_t)STATLIST_UnitGetStatValue(pPlayer, STAT_LEVEL, 0);
    saveHeader.dwCreateTime = CLIENTS_GetCreateTime(pClient);
    saveHeader.dwLasTime = (uint32_t)time(0);
    saveHeader.dwPlayTime = (uint32_t)-1;
    saveHeader.dwMapSeed = pGame->dwInitSeed;
    saveHeader.nStats = 16;
    saveHeader.nSkills = (uint8_t)sgptDataTables->nHighestClassSkillCount;

    if (CLIENTS_GetWeaponSwitch(pClient))
    {
        saveHeader.dwWeaponSwitch |= 1;
    }

    for (i = 0; i < 16; ++i)
    {
        int32_t nSkill = 0;
        int32_t nParam = 0;
        int32_t nGUID = 0;
        CLIENTS_GetSkillHotKey(pClient, i, &nSkill, &nParam, &nGUID);
        saveHeader.SkillKeys[i].nSkill = (uint16_t)(nSkill | (nParam != 0 ? 0x8000 : 0));
        *((int16_t*)&saveHeader.SkillKeys[0].nKey + 2 * i) = PLRSAVE2_GetItemIndex(pPlayer, nGUID);
    }

    D2SkillStrc* pSkill = UNITS_GetLeftSkill(pPlayer);
    if (pSkill)
    {
        saveHeader.nLeftSkillId = (int16_t)SKILLS_GetSkillIdFromSkill(pSkill, __FILE__, __LINE__);
        saveHeader.nLeftSkillItemIndex = PLRSAVE2_GetItemIndex(pPlayer, SKILLS_GetOwnerGUIDFromSkill(pSkill));
    }

    pSkill = UNITS_GetRightSkill(pPlayer);
    saveHeader.nRightSkillId = (int16_t)SKILLS_GetSkillIdFromSkill(pSkill, __FILE__, __LINE__);
    saveHeader.nRightSkillItemIndex = PLRSAVE2_GetItemIndex(pPlayer, SKILLS_GetOwnerGUIDFromSkill(pSkill));

    int32_t nSkillId = 0;
    int32_t nGUID = 0;
    UNITS_GetSwitchLeftSkillData(pPlayer, &nSkillId, &nGUID);
    saveHeader.nSwitchLeftSkillId = (int16_t)nSkillId;
    saveHeader.nSwitchLeftSkillItemIndex = PLRSAVE2_GetItemIndex(pPlayer, nGUID);

    UNITS_GetSwitchRightSkillData(pPlayer, &nSkillId, &nGUID);
    saveHeader.nSwitchRightSkillId = (int16_t)nSkillId;
    saveHeader.nSwitchRightSkillItemIndex = PLRSAVE2_GetItemIndex(pPlayer, nGUID);

    // The act reached, per difficulty, with the high bit marking "this difficulty was entered".
    saveHeader.nTown[pGame->nDifficulty] = (uint8_t)(CLIENTS_GetActNo(pClient) | 0x80);

    memset(&saveHeader.nComponent, 0xFF, 16);
    memset(&saveHeader.nCompColor, 0xFF, 16);
    INVENTORY_GetItemSaveGfxInfo(pPlayer, saveHeader.nComponent, saveHeader.nCompColor);

    PLRSAVE2_WriteMercData(pGame, pPlayer, &saveHeader);

    uint8_t nEmblemBg = 0;
    uint8_t nUnused1 = 0;
    uint8_t nUnused2 = 0;
    CLIENTS_GetGuildEmblem(pClient, &nEmblemBg, &nUnused1, &nUnused2);
    saveHeader.nGuildEmblemBgColor = nEmblemBg;

    memcpy(*ppSection, &saveHeader, sizeof(D2SaveHeaderStrc));
    *ppSection += sizeof(D2SaveHeaderStrc);
    return 0;
}

//D2Game.0x6FC8D0A0 -- the whole file, in retail's section order.
//
// The order is not a guess: PLRSAVE2's own error enum enumerates it (BAD_QUESTRECORD, BAD_WAYPOINT,
// BAD_PLAYERINTRO, BAD_STATS, BAD_SKILLS, BAD_ITEMS, BAD_CORPSES, BAD_PET, BAD_IRONGOLEM), and the
// reader validates in the same sequence.
int32_t __fastcall PLRSAVE2_CreateSaveFile(D2GameStrc* pGame, D2UnitStrc* pPlayer, uint8_t* pData, uint32_t* pSize, uint32_t nMaxSize, int32_t a6, int32_t a7)
{
    *pSize = 0;

    if (!pPlayer || pPlayer->dwUnitType != UNIT_PLAYER)
    {
        return PLRSAVE2ERROR_UNKNOWN;
    }

    uint8_t* const pStart = pData;
    const uint8_t* const pEnd = &pData[nMaxSize];

    uint8_t* ppSection = pData;
    int32_t nResult = PLRSAVE2_WriteSaveHeader(pGame, pPlayer, &ppSection, pEnd);
    if (nResult)
    {
        return nResult;
    }

    nResult = D2WEB_PLRSAVE2_WriteQuestSection(pPlayer, &ppSection, pEnd);
    if (nResult)
    {
        return nResult;
    }

    nResult = PLRSAVE2_WriteWaypointData(pGame, pPlayer, &ppSection, pEnd);
    if (nResult)
    {
        return nResult;
    }

    nResult = PLRSAVE2_WritePlrIntroSection(pGame, pPlayer, &ppSection, pEnd);
    if (nResult)
    {
        return nResult;
    }

    nResult = PLRSAVE2_WritePlayerStats(pGame, pPlayer, &ppSection, pEnd);
    if (nResult)
    {
        return nResult;
    }

    // The skill section is inline in retail: an 'if' marker then one byte per class skill.
    const int32_t nClassId = CLIENTS_GetClassId(SUNIT_GetClientFromPlayer(pPlayer, __FILE__, __LINE__));
    if (nClassId >= 7)
    {
        return PLRSAVE2ERROR_UNKNOWN;
    }
    if (&ppSection[SKILLS_GetPlayerSkillCount(nClassId) + 2] > pEnd)
    {
        return PLRSAVE2ERROR_INVALID_SIZE;
    }

    ppSection[0] = 'i';
    ppSection[1] = 'f';
    ppSection += 2;
    for (int32_t i = 0; i < SKILLS_GetPlayerSkillCount(nClassId); ++i)
    {
        *ppSection = (uint8_t)SKILLS_GetSkillLevel(pPlayer,
            SKILLS_GetSkillById(pPlayer, SKILLS_GetClassSkillId(nClassId, i), (D2UnitGUID)-1), 0);
        ++ppSection;
    }

    // The items section is sub_6FC8A0F0, already vendored: it returns the byte count it wrote, or
    // a negative value on failure.
    const int32_t nItemsSize = sub_6FC8A0F0(pGame, pPlayer, ppSection, (int32_t)(pEnd - ppSection), a6, a7);
    if (nItemsSize < 0)
    {
        return PLRSAVE2ERROR_BAD_ITEMS;
    }
    ppSection += nItemsSize;

    nResult = PLRSAVE2_WriteCorpsesSection(pGame, pPlayer, &ppSection, pEnd, a7);
    if (nResult)
    {
        return nResult;
    }

    // Expansion tail: the mercenary's item block, then the iron golem's. Both are present in every
    // expansion save even when empty, which is why they are written unconditionally here.
    if (pGame->bExpansion)
    {
        if (ppSection + 3 > pEnd)
        {
            return PLRSAVE2ERROR_INVALID_SIZE;
        }
        ppSection[0] = 'j';
        ppSection[1] = 'f';
        ppSection += 2;
        // TODO(client-parity): a hired mercenary's items are not serialised yet -- the marker and an
        // empty block are written so the file's section chain stays well formed and loadable.
        ppSection[0] = 'k';
        ppSection[1] = 'f';
        ppSection[2] = 0;   // iron golem present flag
        ppSection += 3;
    }

    const uint32_t nFileSize = (uint32_t)(ppSection - pStart);
    D2SaveHeaderStrc* pHeader = (D2SaveHeaderStrc*)pStart;
    pHeader->dwSize = nFileSize;
    pHeader->dwChecksum = 0;
    pHeader->dwChecksum = (uint32_t)D2GAME_SAVE_CalculateChecksum_6FC8A140(pHeader, (int32_t)nFileSize);

    *pSize = nFileSize;
    return PLRSAVE2ERROR_SUCCESS;
}

// Load a .d2s back INTO AN EXISTING player, the mirror of PLRSAVE2_CreateSaveFile above.
//
// This is deliberately not a port of PLRSAVE2_ProcessSaveFile. That function does two jobs: it
// CREATES the player unit from the header (PLRSAVE2_ReadSaveHeader allocates the unit and wires it
// to a client) and then applies the sections. The session already owns a live player, so only the
// second half applies here; the first is what a realm server does when a character joins a game.
// The validation and the section order below mirror ProcessSaveFile exactly, which is the part that
// defines the format.
//
// One divergence, recorded because it looks like a decompilation artifact rather than retail
// behaviour: ProcessSaveFile's quest check reads `pSaveHeader->dwHeaderMagic` -- the FILE magic at
// offset 0, which is always 0xAA55AA55 -- and compares it to '!ooW', a test that can never pass. The
// quest marker it means to check lives at the section pointer, which is what the vendored
// D2WEB_PLRSAVE2_ReadQuestSection does. We use the section pointer.
int32_t __fastcall D2WEB_PLRSAVE2_LoadIntoPlayer(D2GameStrc* pGame, D2UnitStrc* pPlayer, uint8_t* pSaveFile, uint32_t nSize)
{
    if (!pGame || !pPlayer || !pSaveFile) return PLRSAVE2ERROR_UNKNOWN;
    if (nSize < sizeof(D2SaveHeaderStrc)) return PLRSAVE2ERROR_INVALID_SIZE;

    D2SaveHeaderStrc* pSaveHeader = (D2SaveHeaderStrc*)pSaveFile;
    if (pSaveHeader->dwHeaderMagic != D2SMAGIC_HEADER) return PLRSAVE2ERROR_NOT_COMPATIBLE;
    if (pSaveHeader->dwVersion != 96) return PLRSAVE2ERROR_NOT_COMPATIBLE;
    if (pSaveHeader->dwSize != nSize) return PLRSAVE2ERROR_INVALID_SIZE;

    // Retail verifies the checksum before trusting any of it.
    const uint32_t nStoredChecksum = pSaveHeader->dwChecksum;
    const uint32_t nComputed = (uint32_t)D2GAME_SAVE_CalculateChecksum_6FC8A140(pSaveHeader, (int32_t)nSize);
    if (nStoredChecksum != nComputed) return PLRSAVE2ERROR_INVALID_CHECKSUM;

    const uint32_t nVersion = pSaveHeader->dwVersion;
    uint8_t* pSection = pSaveFile + sizeof(D2SaveHeaderStrc);
    uint8_t* pSaveFileEnd = pSaveFile + nSize;

    int32_t nErrorCode = D2WEB_PLRSAVE2_ReadQuestSection(pPlayer, &pSection, pSaveFileEnd);
    if (nErrorCode) return nErrorCode;

    nErrorCode = PLRSAVE2_ReadWaypointData(pGame, pPlayer, &pSection, pSaveFileEnd, (int32_t)nVersion);
    if (nErrorCode) return nErrorCode;

    nErrorCode = D2WEB_PLRSAVE2_ReadPlrIntroSection(pPlayer, &pSection, pSaveFileEnd);
    if (nErrorCode) return nErrorCode;

    // ProcessSaveFile calls PLRSAVE2_ReadStats(..., pSaveHeader->nStats); that function delegates
    // straight to ReadStatsEx for any version > 94, and 1.13c saves are version 96, so its other
    // branch (the pre-1.09 fixed-width stat block) is unreachable here and is not vendored.
    nErrorCode = PLRSAVE2_ReadStatsEx(pGame, pPlayer, &pSection, pSaveFileEnd, nVersion);
    if (nErrorCode) return nErrorCode;

    nErrorCode = PLRSAVE2_ReadSkills(pGame, pPlayer, &pSection, pSaveFileEnd, nVersion, pSaveHeader->nSkills);
    if (nErrorCode) return nErrorCode;

    int32_t nSectionSize = 0;
    if (sub_6FC8BC70(pGame, pPlayer, pSection, nVersion, (int32_t)(pSaveFileEnd - pSection), 0, &nSectionSize))
    {
        return PLRSAVE2ERROR_BAD_ITEMS;
    }
    pSection += nSectionSize;

    nErrorCode = PLRSAVE2_ReadCorpses(pGame, pPlayer, &pSection, pSaveFileEnd, nVersion, &nSectionSize);
    if (nErrorCode) return nErrorCode;

    // d2-web: the hands the file holds (header nLeftSkillId 0x78 / nRightSkillId 0x7C, written by the
    // save above from UNITS_GetLeft/RightSkill), assigned as retail's SelectSkill handler (0x3C) does:
    // D2GAME_AssignSkill + UNITS_InitLeft/RightSkill, for a skill the player has at level > 0. An
    // item-granted hand (item index != 0, 1-based) is left to the default until item skills are covered.
    // Retail's own load-time restore is not located yet; s189 asserts the reloaded hand.
    for (int32_t bLeft = 0; bLeft < 2; ++bLeft)
    {
        const int32_t nSkillId = bLeft ? pSaveHeader->nLeftSkillId : pSaveHeader->nRightSkillId;
        const int32_t nItemIndex = bLeft ? pSaveHeader->nLeftSkillItemIndex : pSaveHeader->nRightSkillItemIndex;
        if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount || nItemIndex != 0) continue;   // 0 = no item (PLRSAVE2_GetItemIndex)
        D2SkillStrc* pSkill = SKILLS_GetSkillById(pPlayer, nSkillId, -1);
        if (!pSkill || !SKILLS_GetSkillLevel(pPlayer, pSkill, 1)) continue;
        D2GAME_AssignSkill_6FD13800(pPlayer, bLeft, nSkillId, -1);
        if (bLeft) UNITS_InitLeftSkill(pPlayer); else UNITS_InitRightSkill(pPlayer);
    }

    // The expansion tail (mercenary 'j f', iron golem 'k f') is written as empty markers by
    // CreateSaveFile above and is not read back; see C3d.
    return PLRSAVE2ERROR_SUCCESS;
}
