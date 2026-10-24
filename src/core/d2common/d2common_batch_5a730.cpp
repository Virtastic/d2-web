// Prior art: D2MOO DataTbls, MonsterTbls, LevelsTbls, Items and DrlgTileSub;
// re-derived against the twelve cached 1.13c bodies. Register-only arguments are explicit.
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <emscripten.h>
#include <Archive.h>
#include <Fog.h>
#include "D2BitManip.h"
#include "D2DataTbls.h"
#include "D2Items.h"
#include "D2Monsters.h"
#include "D2StatList.h"
#include "DataTbls/MonsterTbls.h"
#include "DataTbls/LevelsTbls.h"
#include "Drlg/D2DrlgDrlgGrid.h"
#include "Drlg/D2DrlgPreset.h"
#include "Units/Units.h"

static_assert(sizeof(D2StatesTxt) == 0x3c);
static_assert(offsetof(D2StatesTxt, wState) == 0);
static_assert(offsetof(D2StatesTxt, nStateFlags) == 0x10);
static_assert(sizeof(D2LevelDefBin) == 0x9c);
static_assert(offsetof(D2LevelDefBin, dwPortal) == 0x8c);
static_assert(sizeof(D2ItemsTxt) == 0x1a8);

// 1.13c D2Common+0x5A730
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_5a730(HD2ARCHIVE archive)
{
    sgptDataTables->pTreasureClassExLinker = (D2TxtLinkStrc*)FOG_AllocLinker(__FILE__, 0x60e);
    DATATBLS_CreateItemTypeTreasureClasses();
    DATATBLS_LoadTreasureClassExTxt(archive);
    const char* difficulty[] = {"", " (N)", " (H)"};
    const char* variant[] = {"A", "B", "C"};
    for (int d = 0; d != 3; ++d)
        for (int act = 0; act != 5; ++act)
            for (int v = 0; v != 3; ++v) {
                char name[256];
                std::snprintf(name, sizeof name, "Act %d%s Chest %s", act + 1, difficulty[d], variant[v]);
                const int row = FOG_GetRowFromTxt(sgptDataTables->pTreasureClassExLinker, name, 0);
                sgptDataTables->pChestTreasureClasses[v + 3 * (act + 5 * d)] = row < 0 ? nullptr : &sgptDataTables->pTreasureClassEx[row];
            }
    if (sgptDataTables->nTreasureClassEx >= 65535) std::exit(-1);
}

// 1.13c D2Common+0x5D9B0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_5d9b0(int16_t** output, int16_t* count)
{
    *output = (int16_t*)D2_CALLOC(uint32_t(sgptDataTables->nStatesTxtRecordCount) * 2u);
    *count = 0;
    for (int i = 0; i < sgptDataTables->nStatesTxtRecordCount; ++i)
        if (int16_t(sgptDataTables->pStatesTxt[i].wState) > 0) {
            (*output)[*count] = int16_t(i);
            *count = int16_t(uint16_t(*count) + 1u);
        }
}

// 1.13c D2Common+0x5DA30
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_5da30(int32_t bit, int16_t** output, int16_t* count)
{
    *output = (int16_t*)D2_CALLOC(uint32_t(sgptDataTables->nStatesTxtRecordCount) * 2u);
    *count = 0;
    for (int i = 0; i < sgptDataTables->nStatesTxtRecordCount; ++i)
        if (sgptDataTables->pStatesTxt[i].nStateFlags[bit >> 3] & uint8_t(1u << (bit & 7))) {
            (*output)[*count] = int16_t(i);
            *count = int16_t(uint16_t(*count) + 1u);
        }
}

// 1.13c D2Common+0x5DAD0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_5dad0(int32_t bit, uint32_t* output)
{
    for (int32_t i = 0; i < sgptDataTables->nStatesTxtRecordCount; ++i)
        if (sgptDataTables->pStatesTxt[i].nStateFlags[bit >> 3] & uint8_t(1u << (bit & 7)))
            output[i >> 5] |= 1u << (i & 31);
}

// 1.13c D2Common+0x6C280
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_6c280(const char* name)
{
    static const char* names[] = {"fl", "wl", "wr", "wtlr", "wtll", "wtr", "wbl", "wbr", "wld", "wrd", "wle", "wre", "co", "sh", "tr", "rf", "ld", "rd", "fd", "fi"};
    if (*name == '0') return 0;
    for (int i = 0; i != 20; ++i) if (!std::strcmp(name, names[i])) return i;
    std::exit(-1);
}

// 1.13c D2Common+0x6C300
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_6c300(const char* name)
{
    static const char* names[] = {"None", "1 Town", "1 Wilderness", "1 Cave", "1 Crypt", "1 Monestary", "1 Courtyard", "1 Barracks", "1 Jail", "1 Cathedral", "1 Catacombs", "1 Tristram", "2 Town", "2 Sewer", "2 Harem", "2 Basement", "2 Desert", "2 Tomb", "2 Lair", "2 Arcane", "3 Town", "3 Jungle", "3 Kurast", "3 Spider", "3 Dungeon", "3 Sewer", "4 Town", "4 Mesa", "4 Lava", "5 Town", "5 Siege", "5 Barricade", "5 Temple", "5 Ice", "5 Baal", "5 Lava"};
    if (*name == '0') return 0;
    for (int i = 0; i != 36; ++i) if (!std::strcmp(name, names[i])) return i;
    std::exit(-1);
}

// 1.13c D2Common+0x6C710
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_6c710(const char* prefix, char* file)
{
    const size_t length = std::strlen(file);
    for (size_t i = 0; i < length; ++i) if (file[i] == '/') file[i] = '\\';
    if (length > 1) {
        char path[60];
        std::sprintf(path, "%s\\%s", prefix, file);
        std::strcpy(file, path);
    }
}

// 1.13c D2Common+0x6CB50
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_6cb50(const D2LevelDefBin* records)
{
    sgptDataTables->nPortalLevels = 0;
    for (int i = 0; i < sgptDataTables->nLevelsTxtRecordCount; ++i)
        if (records[i].dwPortal) ++sgptDataTables->nPortalLevels;
    if (sgptDataTables->nPortalLevels)
        sgptDataTables->pPortalLevels = (int*)D2_CALLOC(uint32_t(sgptDataTables->nPortalLevels) * 4u);
    int next = 0;
    for (int i = 0; i < sgptDataTables->nLevelsTxtRecordCount; ++i)
        if (records[i].dwPortal) {
            if (next >= sgptDataTables->nPortalLevels) std::exit(-1);
            sgptDataTables->pPortalLevels[next++] = i;
        }
}

// 1.13c D2Common+0x710F0
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_710f0(HD2ARCHIVE archive, const char* name, const char* extension)
{
    char path[260];
    std::sprintf(path, "%s\\%s%s", "DATA\\GLOBAL\\EXCEL", name, extension);
    HSFILE file = nullptr;
    if (!ARCHIVE_OpenFile(archive, path, &file, TRUE)) {
        if (ARCHIVE_GetLastError() != 2) FOG_Trace("Error opening file: %s", path);
        return 0;
    }
    if (!file) std::exit(-1);  // retail: null handle after successful open is fatal
    ARCHIVE_CloseFile(archive, file);
    return 1;
}

// 1.13c D2Common+0x1F980
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_1f980(D2UnitStrc* unit, D2UnitStrc* first, D2UnitStrc* second)
{
    if (!first || !second) return 1;
    if (ITEMS_GetAmmoType(first) && ITEMS_CheckItemTypeId(second, ITEMS_GetAmmoType(first))) return 1;
    if (ITEMS_GetAmmoType(second) && ITEMS_CheckItemTypeId(first, ITEMS_GetAmmoType(second))) return 1;
    if (ITEMS_GetQuiverType(first) || ITEMS_GetQuiverType(second)) return 0;
    if (ITEMS_CheckWeaponIfTwoHanded(first) && !ITEMS_Is1Or2Handed(unit, first)) return 0;
    if (ITEMS_CheckWeaponIfTwoHanded(second) && !ITEMS_Is1Or2Handed(unit, second)) return 0;
    if (!ITEMS_CheckItemTypeId(first, 0x2d) || !ITEMS_CheckItemTypeId(second, 0x2d))
        return ITEMS_CheckItemTypeId(first, 0x2d) || ITEMS_CheckItemTypeId(second, 0x2d);
    if (!unit) return 0;
    if (unit->dwUnitType == 0) {
        if (unit->dwClassId == 4) return 1;
        if (unit->dwClassId != 6) return 0;
    } else if (unit->dwUnitType == 1) {
        switch (MONSTERS_ValidateMonsterId(int32_t(unit->dwClassId))) {
            case 0x21c: case 0x21d: case 0x21e: return 1;
            case 0x1a1: case 0x1a2: break;
            default: return 0;
        }
    } else return 0;
    return ITEMS_CheckItemTypeId(first, 0x43) && ITEMS_CheckItemTypeId(second, 0x43);
}

// 1.13c D2Common+0x25C10
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_25c10(D2UnitStrc* item, D2BitBufferStrc* buffer, int32_t server)
{
    if (!item || uint32_t(item->dwClassId) >= uint32_t(sgptDataTables->pItemDataTables.nItemsTxtRecordCount) || !sgptDataTables->pItemDataTables.pItemsTxt
        || item->dwUnitType != 4 || !item->pItemData) std::exit(-1);
    const D2ItemsTxt* row = &sgptDataTables->pItemDataTables.pItemsTxt[item->dwClassId];
    auto write = [buffer](int32_t value, int bits) { ITEMS_WriteBitsToBitstream(buffer, value, bits); };
    write(uint32_t(std::clamp<int>(ITEMS_GetItemFormat(item), 0, 1023)), 10);
    write(item->dwAnimMode, 3);
    if (item->dwAnimMode == 3 || item->dwAnimMode == 5) {
        write(UNITS_GetXPosition(item), 16); write(UNITS_GetYPosition(item), 16);
    } else {
        write(ITEMS_GetBodyLocation(item), 4);
        write(std::clamp(UNITS_GetXPosition(item), 0, 15), 4);
        write(std::clamp(UNITS_GetYPosition(item), 0, 15), 4);
        // Retail increments AL, then MOVSX before the saturating write.
        write(int8_t(uint8_t(ITEMS_GetInvPage(item) + 1)), 3);
    }
    if (ITEMS_GetItemFlags(item) & 0x10000) {
        write(std::clamp(ITEMS_GetFileIndex(item), 0, 7), 3);
        write(ITEMS_GetEarLevel(item), 7);
        const char* name = ITEMS_GetEarName(item);
        do { write(*name, 7); } while (*name++);
    } else {
        write(ITEMS_GetBaseCode(item), 32);
        if (ITEMS_CheckItemTypeId(item, 4)) {
            const int32_t gold = STATLIST_UnitGetStatValue(item, 14, 0);
            write(gold >= 4096, 1); write(gold, gold < 4096 ? 12 : 32);
        }
    }
    if (row->nQuest && row->nQuestDiffCheck) {
        if (sgptDataTables->nItemStatCostTxtRecordCount < 0x165 || !sgptDataTables->pItemStatCostTxt) std::exit(-1);
        const auto& stat = sgptDataTables->pItemStatCostTxt[0x164];
        const int value = STATLIST_UnitGetStatValue(item, 0x164, 0);
        ITEMS_WriteBitsToBitstream(buffer, uint32_t(value >> (stat.nValShift & 31)) + stat.dwSaveAdd, stat.nSaveBits);
    }
    if (server) {
        int realm0 = 0, realm1 = 0;
        ITEMS_GetRealmData(item, &realm0, &realm1);
        write(realm1 != 0, 1);
        if (realm1) { write(realm0, 32); write(realm1, 32); write(0, 32); }
    }
}

// 1.13c D2Common+0x833F0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_833f0(D2LvlSubTxt* record)
{
    if (!record->pDrlgFile) return;
    for (int i = 0; i < record->pDrlgFile->nWallLayers; ++i) {
        DRLGGRID_FreeGrid(nullptr, &record->pWallGrid[i]);
        DRLGGRID_FreeGrid(nullptr, &record->pTileTypeGrid[i]);
    }
    DRLGGRID_FreeGrid(nullptr, &record->pFloorGrid);
    DRLGGRID_FreeGrid(nullptr, &record->pShadowGrid);
    DRLGPRESET_FreeDrlgFile(&record->pDrlgFile);
}
