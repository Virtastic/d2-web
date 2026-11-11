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
uint32_t D2Common_24430_CheckItemTypeId(const D2UnitStrc* pItem, int nItemType);
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

// Superseded by d2common_5a730.cpp (the 1.13c body); kept only until the batch is retired.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_port_d2common_5a730(HD2ARCHIVE archive)
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

// 1.13c D2Common+0x5DAD0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_5dad0_batch(int32_t bit, uint32_t* output)
{
    for (int32_t i = 0; i < sgptDataTables->nStatesTxtRecordCount; ++i)
        if (sgptDataTables->pStatesTxt[i].nStateFlags[bit >> 3] & uint8_t(1u << (bit & 7)))
            output[i >> 5] |= 1u << (i & 31);
}

// 1.13c D2Common+0x6C280
int32_t D2Common_6C280_NameIndex(const char* name)
{
    static const char* names[] = {"fl", "wl", "wr", "wtlr", "wtll", "wtr", "wbl", "wbr", "wld", "wrd", "wle", "wre", "co", "sh", "tr", "rf", "ld", "rd", "fd", "fi"};
    if (*name == '0') return 0;
    for (int i = 0; i != 20; ++i) if (!std::strcmp(name, names[i])) return i;
    std::exit(-1);
}

// 1.13c D2Common+0x6C300
int32_t D2Common_6C300_NameIndex(const char* name)
{
    static const char* names[] = {"None", "1 Town", "1 Wilderness", "1 Cave", "1 Crypt", "1 Monestary", "1 Courtyard", "1 Barracks", "1 Jail", "1 Cathedral", "1 Catacombs", "1 Tristram", "2 Town", "2 Sewer", "2 Harem", "2 Basement", "2 Desert", "2 Tomb", "2 Lair", "2 Arcane", "3 Town", "3 Jungle", "3 Kurast", "3 Spider", "3 Dungeon", "3 Sewer", "4 Town", "4 Mesa", "4 Lava", "5 Town", "5 Siege", "5 Barricade", "5 Temple", "5 Ice", "5 Baal", "5 Lava"};
    if (*name == '0') return 0;
    for (int i = 0; i != 36; ++i) if (!std::strcmp(name, names[i])) return i;
    std::exit(-1);
}

// 1.13c D2Common+0x6C710
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_6C710_PrefixPath(const char* prefix, char* file)
{
    const size_t length = std::strlen(file);
    for (size_t i = 0; i < length; ++i) if (file[i] == '/') file[i] = '\\';
    if (length > 1) {
        char path[60];
        std::sprintf(path, "%s\\%s", prefix, file);
        std::strcpy(file, path);
    }
}

// 1.13c D2Common+0x6CB50: the portal-level index list. The allocator is a seam so the replay can hook it.
extern "C" void d2_seam_d2common_6cb50(const D2LevelDefBin* records, int count, int* nOut, int** listOut, void* (*alloc)(uint32_t size))
{
    *nOut = 0;
    for (int i = 0; i < count; ++i)
        if (records[i].dwPortal) ++*nOut;
    if (*nOut)
        *listOut = (int*)alloc(uint32_t(*nOut) * 4u);
    int next = 0;
    for (int i = 0; i < count; ++i)
        if (records[i].dwPortal) {
            if (next >= *nOut) std::exit(-1);
            (*listOut)[next++] = i;
        }
}
extern "C" void d2_port_d2common_6cb50(const D2LevelDefBin* records)
{
    d2_seam_d2common_6cb50(records, sgptDataTables->nLevelsTxtRecordCount, &sgptDataTables->nPortalLevels, &sgptDataTables->pPortalLevels,
        [](uint32_t size) -> void* { return D2_CALLOC(size); });
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
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_port_d2common_1f980(D2UnitStrc* unit, D2UnitStrc* first, D2UnitStrc* second)
{
    // Item-type tests are the 1.13c 0x24430 form (D2Common_24430_CheckItemTypeId), not the 1.10f ITEMS_CheckItemTypeId.
    if (!first || !second) return 1;
    if (ITEMS_GetAmmoType(first) && D2Common_24430_CheckItemTypeId(second, ITEMS_GetAmmoType(first))) return 1;
    if (ITEMS_GetAmmoType(second) && D2Common_24430_CheckItemTypeId(first, ITEMS_GetAmmoType(second))) return 1;
    if (ITEMS_GetQuiverType(first) || ITEMS_GetQuiverType(second)) return 0;
    if (ITEMS_CheckWeaponIfTwoHanded(first) && !ITEMS_Is1Or2Handed(unit, first)) return 0;
    if (ITEMS_CheckWeaponIfTwoHanded(second) && !ITEMS_Is1Or2Handed(unit, second)) return 0;
    if (!D2Common_24430_CheckItemTypeId(first, 0x2d) || !D2Common_24430_CheckItemTypeId(second, 0x2d))
        return D2Common_24430_CheckItemTypeId(first, 0x2d) || D2Common_24430_CheckItemTypeId(second, 0x2d);
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
    return D2Common_24430_CheckItemTypeId(first, 0x43) && D2Common_24430_CheckItemTypeId(second, 0x43);
}

// 1.13c D2Common+0x25C10: moved to d2common_25c10.cpp (seam-based port).

// 1.13c D2Common+0x833F0: LvlSub teardown. Retail does not free the grids: for each grid it asserts the
// row-offsets pointer is null (Fog assert, DrlgGrid.cpp line 0x218), then zeroes flags and row offsets.
// Order: per layer wall (+0xA4/+0xA8) then tile (+0x54/+0x58), then floor, then shadow, then the drlg file.
extern "C" {
#define W __attribute__((weak))
W void Ordinal_10046(const char* file, int32_t line, int32_t z);
}
static void b5a730_zeroGrid(D2DrlgGridStrc* g)
{
    if (g->pCellsRowOffsets) Ordinal_10046("..\\Source\\D2Common\\DRLG\\DrlgGrid.cpp", 0x218, 0);
    g->pCellsFlags = nullptr;
    g->pCellsRowOffsets = nullptr;
}
void D2Common_833F0_LvlSubTeardown(D2LvlSubTxt* record)
{
    if (!record->pDrlgFile) return;
    for (int i = 0; i < record->pDrlgFile->nWallLayers; ++i) {
        b5a730_zeroGrid(&record->pWallGrid[i]);
        b5a730_zeroGrid(&record->pTileTypeGrid[i]);
    }
    b5a730_zeroGrid(&record->pFloorGrid);
    b5a730_zeroGrid(&record->pShadowGrid);
    DRLGPRESET_FreeDrlgFile(&record->pDrlgFile);
}
