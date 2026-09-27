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
#include <D2Constants.h>
#include <D2DataTbls.h>
#include <D2StatList.h>
#include <D2Inventory.h>
#include <D2Skills.h>
#include <D2Items.h>               // IMODE_ONGROUND, the mode the golem's restored item is placed in
#include <DataTbls/MonsterIds.h>   // MONSTER_IRONGOLEM, the iron golem section's own class
#include <DataTbls/SkillsIds.h>    // SKILL_IRONGOLEM, the skill the golem's item is carried under
#include <D2Waypoints.h>
#include <D2QuestRecord.h>
#include <Units/Units.h>
#include <GAME/Game.h>
#include <GAME/Arena.h>
#include <GAME/Clients.h>
#include "SKILLS/Skills.h"
#include <D2Monsters.h>            // MONSTERS_GetHirelingExpForNextLevel
#include <PLAYER/PlrSave.h>
#include <PLAYER/PlrSave2.h>
#include <PLAYER/Player.h>
#include <PLAYER/PlayerPets.h>
#include <ITEMS/Items.h>           // sub_6FC4EC10, one item out of a section
#include <ITEMS/ItemMode.h>        // D2GAME_ITEMS_UpdateInventoryItems_6FC45050
#include <MONSTER/Monster.h>       // MONSTER_DeleteEvents
#include <MONSTER/MonsterAI.h>     // MONSTERAI_UpdateMercStatsAndSkills
#include <UNIT/SUnit.h>
#include <UNIT/SUnitMsg.h>         // D2GAME_MERCS_SendStat_6FCC61D0
#include <UNIT/SUnitNpc.h>         // D2GAME_MERCS_Create_6FCC8630
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

    // Expansion tail: the mercenary's item block, then the iron golem's. Both markers are present in
    // every expansion save even when their block is empty, which is why they are written
    // unconditionally here.
    if (pGame->bExpansion)
    {
        if (ppSection + 3 > pEnd)
        {
            return PLRSAVE2ERROR_INVALID_SIZE;
        }
        ppSection[0] = 'j';
        ppSection[1] = 'f';
        ppSection += 2;

        // The hireling's items are written by the SAME writer the player's own section uses
        // (PLRSAVE_WriteItems, retail's sub_6FC8A0F0: a 'JM' marker, a 16-bit count, then one
        // byte-aligned serialized item each), applied to the pet's inventory. A pet that does not
        // exist gets no block -- the marker then stands alone and 'k f' follows it directly, which
        // is what every save we hold does. Writing the marker with nothing behind it unconditionally
        // (which is what this did) dropped every hired mercenary's gear (client-parity C3d).
        D2UnitStrc* pPet = sub_6FC7E8B0(pGame, pPlayer, PETTYPE_HIREABLE, 1);
        if (pPet)
        {
            const int32_t nPetSectionSize = sub_6FC8A0F0(pGame, pPet, ppSection, (int32_t)(pEnd - ppSection), 0, a7);
            if (nPetSectionSize < 0)
            {
                return PLRSAVE2ERROR_BAD_PET;
            }
            ppSection += nPetSectionSize;
        }

        // PLRSAVE2_WriteIronGolemSection (D2Game.0x6FC8D880): the marker, a present flag, and the
        // golem's own item when there is one. The engine summons no iron golem on any path a row
        // drives, so this arm is unasserted -- but the flag is now the golem's state rather than the
        // constant 0 the marker used to carry.
        ppSection[0] = 'k';
        ppSection[1] = 'f';
        uint8_t bHasIronGolem = 0;
        D2UnitStrc* pIronGolemItem = nullptr;
        D2UnitStrc* pIronGolem = sub_6FC7E8B0(pGame, pPlayer, PETTYPE_GOLEM, 1);
        if (pIronGolem && pIronGolem->dwClassId == MONSTER_IRONGOLEM &&
            SKILLS_GetSkillById(pPlayer, SKILL_IRONGOLEM, (D2UnitGUID)-1) && pIronGolem->pInventory)
        {
            pIronGolemItem = INVENTORY_UnitIsItem(INVENTORY_GetFirstItem(pIronGolem->pInventory));
            if (pIronGolemItem)
            {
                bHasIronGolem = 1;
            }
        }
        ppSection[2] = bHasIronGolem;
        ppSection += 3;
        if (bHasIronGolem)
        {
            const int32_t nGolemItemSize = D2GAME_SAVE_SerializeItem_6FC89AD0(pIronGolemItem, ppSection,
                (uint32_t)(pEnd - ppSection), 0, a7);
            // Retail clears the present flag rather than advancing by a failed length; a serializer
            // that wrote nothing leaves the section at its three bytes.
            if (nGolemItemSize <= 0)
            {
                ppSection[-1] = 0;
            }
            else
            {
                ppSection += nGolemItemSize;
            }
        }
    }

    const uint32_t nFileSize = (uint32_t)(ppSection - pStart);
    D2SaveHeaderStrc* pHeader = (D2SaveHeaderStrc*)pStart;
    pHeader->dwSize = nFileSize;
    pHeader->dwChecksum = 0;
    pHeader->dwChecksum = (uint32_t)D2GAME_SAVE_CalculateChecksum_6FC8A140(pHeader, (int32_t)nFileSize);

    *pSize = nFileSize;
    return PLRSAVE2ERROR_SUCCESS;
}

// PLRSAVE2_ReadMercData (D2Game.0x6FC8E730): the header's MercSaveData record, turned back into a
// live hireling. PLRSAVE2_WriteMercData is its mirror, and the pair is what makes a saved
// mercenary survive a save->load->save round trip rather than only the write half.
//
// The record stores the name as an OFFSET into the hireling's name range (the writer subtracts
// wNameFirst), so the reader adds it back and clamps into the range. A record with no seed, no
// experience and no name is "no mercenary" and creates nothing -- which is what every save we hold
// carries, so a character without a hireling is unaffected by the sections below.
D2UnitStrc* __fastcall D2WEB_PLRSAVE2_ReadMercData(D2GameStrc* pGame, D2UnitStrc* pPlayer, D2MercSaveDataStrc* pData, uint32_t dwVersion)
{
    if (!pData->nExperience && !pData->nSeed && !pData->wName)
    {
        return nullptr;
    }

    D2MercSaveDataStrc hirelingData = {};
    memcpy(&hirelingData, pData, sizeof(hirelingData));

    D2HirelingTxt* pHirelingTxtRecord = DATATBLS_GetHirelingTxtRecordFromIdAndLevel(pGame->bExpansion, hirelingData.nHirelingId, 1);
    if (!pHirelingTxtRecord)
    {
        return nullptr;
    }

    hirelingData.wName += pHirelingTxtRecord->wNameFirst;
    if (hirelingData.wName > pHirelingTxtRecord->wNameLast)
    {
        hirelingData.wName = pHirelingTxtRecord->wNameFirst;
    }

    D2UnitStrc* pMerc = D2GAME_MERCS_Create_6FCC8630(pGame, pPlayer, hirelingData.wName, hirelingData.nSeed,
        hirelingData.nHirelingId, pHirelingTxtRecord->dwClass, hirelingData.nFlags & UNITFLAG_ISDEAD);
    if (!pMerc)
    {
        return nullptr;
    }

    D2PetInfoStrc petInfo = {};
    petInfo.nSeed = hirelingData.nSeed;
    petInfo.wName = hirelingData.wName;
    petInfo.nHirelingId = hirelingData.nHirelingId;
    PLAYERPETS_UpdatePetInfo(pPlayer, PETTYPE_HIREABLE, pMerc->dwUnitId, &petInfo);

    int32_t nLevel = 1;
    const int32_t nMaxLevel = DATATBLS_GetMaxLevel(0);

    if (STATLIST_UnitGetStatValue(pMerc, STAT_EXPERIENCE, 0) < hirelingData.nExperience)
    {
        STATLIST_SetUnitStat(pMerc, STAT_EXPERIENCE, (int32_t)hirelingData.nExperience, 0);
    }

    D2GAME_MERCS_SendStat_6FCC61D0(pMerc, STAT_EXPERIENCE, hirelingData.nExperience);

    do
    {
        pHirelingTxtRecord = DATATBLS_GetHirelingTxtRecordFromIdAndLevel(pGame->bExpansion, hirelingData.nHirelingId, nLevel);
        if (!pHirelingTxtRecord)
        {
            SUNIT_RemoveUnit(pGame, pMerc);
            return nullptr;
        }

        ++nLevel;
    }
    while (nLevel <= nMaxLevel && hirelingData.nExperience >= MONSTERS_GetHirelingExpForNextLevel(nLevel, pHirelingTxtRecord->dwExpPerLvl));

    MONSTERAI_UpdateMercStatsAndSkills(pGame, pPlayer, pMerc, nLevel);

    if (hirelingData.nFlags & UNITFLAG_ISDEAD)
    {
        sub_6FC7D470(pGame, pPlayer, pMerc);
        pMerc->dwFlags |= UNITFLAG_ISDEAD;
        SUNIT_SetCombatMode(pGame, pMerc, MONMODE_DEAD);
        MONSTER_DeleteEvents(pGame, pMerc);
    }

    // A hireling's inventory is allocated on demand (there is none on the unit this creates), and
    // the item section that follows needs one.
    if (!pMerc->pInventory)
    {
        INVENTORY_AllocInventory(nullptr, pMerc);
    }

    return pMerc;
}

// PLRSAVE2_ReadPetSection (D2Game.0x6FC8E850): the 'j f' block, applied to the hireling. The block
// is the same item format as the player's own ('JM', a count, byte-aligned items), read by the
// same reader (sub_6FC8BC70), against the mercenary's inventory instead of the player's.
int32_t __fastcall D2WEB_PLRSAVE2_ReadPetSection(D2GameStrc* pGame, D2UnitStrc* pPlayer, D2UnitStrc* pMerc,
    uint8_t** ppSection, uint8_t* pEnd, uint32_t dwVersion, int32_t* pSectionSize)
{
    if (!pGame->bExpansion)
    {
        return 0;
    }

    uint8_t* pData = *ppSection + 2;
    if (pData > pEnd)
    {
        return 0;
    }

    if (*(uint16_t*)*ppSection != 0x666A)   // 'j f'
    {
        if (pSectionSize) *pSectionSize = 101;
        return PLRSAVE2ERROR_BAD_PET;
    }

    // No hireling: the marker stands alone and the iron golem section follows it directly.
    if (!pMerc)
    {
        *ppSection = pData;
        return 0;
    }

    int32_t nLength = 0;
    const int32_t nResult = sub_6FC8BC70(pGame, pMerc, pData, dwVersion, (int32_t)(pEnd - pData), 0, &nLength);
    if (nResult)
    {
        if (pSectionSize) *pSectionSize = nResult;
        return PLRSAVE2ERROR_BAD_PET;
    }

    D2GAME_ITEMS_UpdateInventoryItems_6FC45050(pGame, pMerc, 0, 0);
    if (!SUNIT_IsDead(pMerc))
    {
        STATLIST_SetUnitStat(pMerc, STAT_HITPOINTS, STATLIST_GetMaxLifeFromUnit(pMerc), 0);
    }

    *ppSection = &pData[nLength];
    return 0;
}

// PLRSAVE2_CreateItem (D2Game.0x6FC8E920): one serialized item out of a section, socketed children
// included. Read by the iron golem section below to restore the golem's item; the player's and the
// mercenary's own item sections use the bulk reader (PLRSAVE_ReadItems), which is a different
// function. Nothing in this engine summons an iron golem, so this is ported for the section chain's
// sake and is unasserted -- see C3d.
int32_t __fastcall D2WEB_PLRSAVE2_CreateItem(D2GameStrc* pGame, D2UnitStrc* pPlayer, D2UnitStrc** ppItem,
    uint8_t** ppSection, uint8_t* pEnd, uint32_t dwVersion)
{
    *ppItem = nullptr;

    uint8_t* pSection = *ppSection;
    uint32_t nSize = (uint32_t)(pEnd - pSection);

    D2ItemSaveStrc pItemSave = {};
    ITEMS_GetCompactItemDataFromBitstream(pSection, nSize, 1, &pItemSave);
    if (pItemSave.nClassId == -1)
    {
        return 0;
    }

    D2UnitStrc* pItem = sub_6FC4EC10(pGame, 0, pSection, nSize, 1, &pItemSave, &nSize, dwVersion);
    if (!pItem)
    {
        return 0;
    }

    uint8_t* pNext = &pSection[nSize];

    for (int32_t i = 0; i < pItemSave.nItemFileIndex; ++i)
    {
        nSize = (uint32_t)(pEnd - pNext);
        ITEMS_GetCompactItemDataFromBitstream(pNext, nSize, 1, &pItemSave);

        D2UnitStrc* pSocketable = sub_6FC4EC10(pGame, 0, pNext, nSize, 1, &pItemSave, &nSize, dwVersion);
        if (!pSocketable)
        {
            SUNIT_RemoveUnit(pGame, pItem);
            return 0;
        }

        pNext += nSize;
        UNITS_ChangeAnimMode(pSocketable, IMODE_ONCURSOR);

        int32_t bPlaced = 0;
        if (!D2GAME_ITEMSOCKET_PlaceItem_6FC497E0(pGame, pPlayer, pSocketable->dwUnitId,
                pItem ? pItem->dwUnitId : -1, &bPlaced, 0, 1, 0))
        {
            SUNIT_RemoveUnit(pGame, pItem);
            return 0;
        }
    }

    if ((ITEMS_GetItemFlags(pItem) & IFLAG_RUNEWORD) && !ITEMS_GetRunesTxtRecordFromItem(pItem))
    {
        D2GAME_ITEMMODE_Unk_6FC4BC10(pGame, pPlayer, pItem);
        pItem = nullptr;
    }

    *ppSection = pNext;
    *ppItem = pItem;
    return 1;
}

// PLRSAVE2_ReadIronGolemSection (D2Game.0x6FC8EAB0): the 'k f' marker, a present flag, and the
// golem's own item when the flag is set. The flag is 0 in every save we hold, so the section is the
// three bytes the writer emits; the item arm is ported for the chain's sake and is unasserted.
int32_t __fastcall D2WEB_PLRSAVE2_ReadIronGolemSection(D2GameStrc* pGame, D2UnitStrc* pPlayer, uint8_t** ppSection,
    uint8_t* pEnd, uint32_t dwVersion)
{
    if (!pGame->bExpansion)
    {
        return 0;
    }

    uint8_t* pData = *ppSection + 2;
    if (pData > pEnd)
    {
        return 0;
    }

    if (*(uint16_t*)*ppSection != 0x666B)   // 'k f'
    {
        return PLRSAVE2ERROR_BAD_IRONGOLEM;
    }

    uint8_t* pSectionEnd = pData + 1;
    if (*pData)
    {
        D2SkillStrc* pSkill = SKILLS_GetHighestLevelSkillFromUnitAndId(pPlayer, SKILL_IRONGOLEM);
        if (!pSkill)
        {
            return PLRSAVE2ERROR_BAD_IRONGOLEM;
        }

        UNITS_SetUsedSkill(pPlayer, pSkill);

        D2UnitStrc* pItem = nullptr;
        if (!D2WEB_PLRSAVE2_CreateItem(pGame, pPlayer, &pItem, &pSectionEnd, pEnd, dwVersion))
        {
            return PLRSAVE2ERROR_BAD_IRONGOLEM;
        }

        UNITS_ChangeAnimMode(pItem, IMODE_ONGROUND);
        CLIENTS_SetIronGolemItemGUID(SUNIT_GetClientFromPlayer(pPlayer, __FILE__, __LINE__), pItem ? (int32_t)pItem->dwUnitId : -1);
    }

    *ppSection = pSectionEnd;
    return 0;
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

    // The expansion tail, in retail's own order (PLRSAVE2_ProcessSaveFile): the header's mercenary
    // record first, which CREATES the hireling, then the 'j f' item block applied to it, then the
    // 'k f' iron golem section. Each function answers 0 in a classic game. Skipping the tail (which
    // is what this did) dropped a mercenary and its gear on load, so a file this engine wrote could
    // not be read back as the character that wrote it (client-parity C3d).
    D2UnitStrc* pMerc = D2WEB_PLRSAVE2_ReadMercData(pGame, pPlayer, &pSaveHeader->MercSaveData, nVersion);
    nErrorCode = D2WEB_PLRSAVE2_ReadPetSection(pGame, pPlayer, pMerc, &pSection, pSaveFileEnd, nVersion, &nSectionSize);
    if (nErrorCode) return nErrorCode;
    if (pMerc)
    {
        MONSTERAI_SendMercStats(pGame, pPlayer, 1);
    }
    nErrorCode = D2WEB_PLRSAVE2_ReadIronGolemSection(pGame, pPlayer, &pSection, pSaveFileEnd, nVersion);
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

    return PLRSAVE2ERROR_SUCCESS;
}

// 1.12f/1.13c PLRSAVE2_CheckPlayerFlags (1.13c D2Game.0x6FD0BAF0). The file's own save flags must
// agree with the game it is joining: expansion/classic, ladder/non-ladder, hardcore/softcore, and
// the difficulty gate against the acts the file records as completed.
int32_t __fastcall PLRSAVE2_CheckPlayerFlags(D2GameStrc* pGame, uint32_t dwFlags)
{
    if (dwFlags & CLIENTSAVEFLAG_EXPANSION)
    {
        if (!pGame->bExpansion) return PLRSAVE2ERROR_NOTEXPANSIONGAME;
    }
    else
    {
        if (pGame->bExpansion) return PLRSAVE2ERROR_EXPANSIONGAME;
    }

    // 1.13c also compares the file's creation stamp with the current ladder start. We run no ladder
    // server, so the constant arm D2MOO compiles for 1.13c ("the stamp is current") is the one that
    // applies: the file's ladder bit is checked, the timestamp is not.
    if (dwFlags & CLIENTSAVEFLAG_LADDER)
    {
        if (!pGame->dwGameType) return PLRSAVE2ERROR_NOTLADDERGAME;
    }
    else
    {
        if (pGame->dwGameType) return PLRSAVE2ERROR_LADDERGAME;
    }

    if (dwFlags & CLIENTSAVEFLAG_HARDCORE)
    {
        if (dwFlags & CLIENTSAVEFLAG_DEAD) return PLRSAVE2ERROR_DEADHARDCORE;
        if (!(ARENA_GetFlags(pGame) & GAMEFLAG_ARENA_HARDCORE)) return PLRSAVE2ERROR_HARDCOREJOINSOFTCORE;
    }
    else
    {
        if (ARENA_GetFlags(pGame) & GAMEFLAG_ARENA_HARDCORE) return PLRSAVE2ERROR_SOFTCOREJOINHARDCORE;
    }

    const uint8_t nProgression = (dwFlags & CLIENTSAVEFLAG_CHARACTER_PROGRESSION_MASK) >> CLIENTSAVEFLAG_CHARACTER_PROGRESSION_BIT;
    switch (pGame->nDifficulty)
    {
    case DIFFMODE_NORMAL:
        break;
    case DIFFMODE_NIGHTMARE:
        if (((dwFlags & CLIENTSAVEFLAG_EXPANSION) && nProgression < 5) || nProgression < 4)
        {
            return PLRSAVE2ERROR_NIGHTMARE_NOT_UNLOCKED;
        }
        break;
    case DIFFMODE_HELL:
        if (((dwFlags & CLIENTSAVEFLAG_EXPANSION) && nProgression < 10) || nProgression < 8)
        {
            return PLRSAVE2ERROR_HELL_NOT_UNLOCKED;
        }
        break;
    default:
        break;
    }

    return 0;
}

// PLRSAVE2_ReadSaveHeader (1.13c D2Game.0x6FD0D250), the half that must run before any character
// exists: validate the file and derive who it is. Retail does this after the game exists -- a realm
// creates the game and the character joins it -- and allocates the unit in the same call. Here the
// header is what decides the game: the act it records is the act the level must be, so the session
// reads it first and creates from it. The unit's class comes out of here, not from the caller.
//
// Two things retail's own function does that are deliberately not here, both because they need a
// live client record and a tuned game rather than a byte buffer:
//   - the `_strcmpi(CLIENTS_GetName(pClient), szName)` check, which the session performs on the
//     client it built (see d2_session_create_from_save);
//   - PLRSAVE2_CheckPlayerFlags, which the session calls once the game it created exists.
// Everything that is a property of the FILE is here, in retail's order: signature, checksum, size,
// version, the name terminator, the class bound, the newbie-save refusal, and the acts and map seed.
int32_t __fastcall D2WEB_PLRSAVE2_ReadSaveHeader(uint8_t* pSaveFile, uint32_t nSize, int32_t nDifficulty, D2SaveHeaderReadStrc* pOut)
{
    if (pOut) { pOut->pHeader = nullptr; pOut->dwMapSeed = 0; pOut->nAct = ACT_I; pOut->nClass = 0; }
    if (!pSaveFile || !pOut) return PLRSAVE2ERROR_CLIENT_ERROR;
    if (nDifficulty < 0 || nDifficulty >= NUM_DIFFICULTIES) return PLRSAVE2ERROR_CLIENT_ERROR;
    if (nSize < sizeof(D2SaveHeaderStrc)) return PLRSAVE2ERROR_CLIENT_ERROR;

    D2SaveHeaderStrc* pSaveHeader = (D2SaveHeaderStrc*)pSaveFile;

    // The file signature is not read inside retail's ReadSaveHeader -- its caller has already
    // refused a file that is not a save -- but the same check stands in D2WEB_PLRSAVE2_LoadIntoPlayer,
    // and keeping it here is what makes both halves answer NOT_COMPATIBLE for the same input.
    if (pSaveHeader->dwHeaderMagic != D2SMAGIC_HEADER) return PLRSAVE2ERROR_NOT_COMPATIBLE;

    // Retail zeroes the checksum field and sums the WHOLE remaining buffer, which at the file's
    // start is the whole file. The checksum routine restores the field itself.
    const uint32_t nCheckSum = pSaveHeader->dwChecksum;
    if ((uint32_t)D2GAME_SAVE_CalculateChecksum_6FC8A140(pSaveHeader, (int32_t)nSize) != nCheckSum)
    {
        return PLRSAVE2ERROR_INVALID_CHECKSUM;
    }

    if (nSize != pSaveHeader->dwSize) return PLRSAVE2ERROR_INVALID_SIZE;

    const uint32_t nVersion = pSaveHeader->dwVersion;
    if (nVersion <= 91 || nVersion > 96) return PLRSAVE2ERROR_NOT_COMPATIBLE;

    pSaveHeader->szName[15] = '\0';

    const uint8_t nClass = pSaveHeader->nClass;
    if (nClass > NUMBER_OF_PLAYERCLASSES) return PLRSAVE2ERROR_CLIENT_ERROR;

    // A newbie save carries the character-creation flag instead of a played character: retail
    // returns here, BEFORE it allocates the unit, and the client is asked to create a fresh
    // character of that class rather than load one. Nothing to load, so nothing is created.
    if (pSaveHeader->tPackedSaveFlags.bInit)
    {
        pOut->nClass = nClass;
        return PLRSAVE2ERROR_NEWBIE_SAVE;
    }

    // nTown[difficulty] holds the act the character was last in for that difficulty, with the top
    // bit marking that the difficulty was entered at all. An act outside the real range is read as
    // Act I, which is retail's own clamp.
    const uint8_t nCurrentTown = pSaveHeader->nTown[nDifficulty];
    uint8_t nAct = nCurrentTown & 0x7F;
    if (nAct >= NUM_ACTS) nAct = ACT_I;

    // Retail writes the file's map seed into the game when the difficulty was entered, so a resumed
    // character walks the map it was saved on (pGame->nGameType == 3 is the single-player game this
    // is: dwInitSeed is only overwritten for a game that has not been seeded yet).
    pOut->dwMapSeed = (nCurrentTown & 0x80) ? pSaveHeader->dwMapSeed : 0;
    pOut->pHeader = pSaveHeader;
    pOut->nAct = nAct;
    pOut->nClass = nClass;
    return PLRSAVE2ERROR_SUCCESS;
}

// The unit half of PLRSAVE2_ReadSaveHeader: what the header says about the character, applied to
// the client and the player unit the session allocated with the class the reader read. Retail does
// these inline in ReadSaveHeader between the unit allocation and the return; the seam is the
// allocation itself, which in this session belongs to the attach that stands the unit in a room.
int32_t __fastcall D2WEB_PLRSAVE2_ApplySaveHeader(D2ClientStrc* pClient, D2UnitStrc* pPlayer, const D2SaveHeaderStrc* pHeader, int32_t nAct)
{
    if (!pClient || !pPlayer || !pHeader) return PLRSAVE2ERROR_BAD_CLIENT;

    D2PlayerDataStrc* pPlayerData = UNITS_GetPlayerData(pPlayer);
    if (!pPlayerData) return PLRSAVE2ERROR_CLIENT_ERROR;

    // A freshly loaded character is in no trade.
    pPlayerData->pTrade = nullptr;
    pPlayerData->dwTradeState = 0;
    pPlayerData->dwAcceptTradeTick = 0;
    pPlayerData->unk0x54 = 0;

    CLIENTS_SetWeaponSwitch(pClient, pHeader->dwWeaponSwitch & 1);
    CLIENTS_SetFlags(pClient, pHeader->dwSaveFlags);
    CLIENTS_SetCreateTime(pClient, pHeader->dwCreateTime);
    CLIENTS_SetActNo(pClient, (uint8_t)nAct);

    for (int32_t i = 0; i < 16; ++i)
    {
        if (pHeader->SkillKeys[i].nSkill == uint16_t(-1))
        {
            CLIENTS_SetSkillHotKey(pClient, i, -1, 0, -1);
            continue;
        }

        const int16_t nSkillData = pHeader->SkillKeys[i].nSkill;
        const int16_t nSkillId = nSkillData & 0x0FFF;
        const uint8_t nHand = (uint8_t)(nSkillData >> 15);

        int16_t nFlags = pHeader->SkillKeys[i].nItemSlot;
        if (!nFlags) nFlags = -1;

        CLIENTS_SetSkillHotKey(pClient, i, nSkillId, nHand, nFlags);
    }

    // The hands. -1 is "no skill"; otherwise the low 12 bits are the skill and the item index is a
    // 1-based inventory index, where 0 means "not an item-granted hand" and stores as -1.
    struct { int16_t nSkill; int16_t nItemIndex; int32_t* pId; int32_t* pFlags; } hands[4] = {
        { pHeader->nLeftSkillId, pHeader->nLeftSkillItemIndex, &pPlayerData->nLeftSkillId, &pPlayerData->nLeftSkillFlags },
        { pHeader->nRightSkillId, pHeader->nRightSkillItemIndex, &pPlayerData->nRightSkillId, &pPlayerData->nRightSkillFlags },
        { pHeader->nSwitchLeftSkillId, pHeader->nSwitchLeftSkillItemIndex, &pPlayerData->nSwitchLeftSkillId, &pPlayerData->nSwitchLeftSkillFlags },
        { pHeader->nSwitchRightSkillId, pHeader->nSwitchRightSkillItemIndex, &pPlayerData->nSwitchRightSkillId, &pPlayerData->nSwitchRightSkillFlags },
    };

    for (int32_t i = 0; i < 4; ++i)
    {
        if (hands[i].nSkill == -1)
        {
            *hands[i].pId = -1;
            *hands[i].pFlags = -1;
            continue;
        }

        *hands[i].pId = hands[i].nSkill & 0x0FFF;
        *hands[i].pFlags = hands[i].nItemIndex;
        if (!*hands[i].pFlags) *hands[i].pFlags = -1;
    }

    CLIENTS_SetGuildEmblem(pClient, pHeader->nGuildEmblemBgColor, 0, 0);
    return PLRSAVE2ERROR_SUCCESS;
}
