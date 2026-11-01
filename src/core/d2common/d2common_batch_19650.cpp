// Stage A, 1.13c cached decompiles cross-checked with D2MOO DataTbls,
// D2Inventory, D2States and DRLG tile defaults. Typed adapters reuse the
// vendored algorithms; register arguments lost by Ghidra are explicit here.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include "D2DataTbls.h"
#include "D2Inventory.h"
#include "D2States.h"
#include "D2StatList.h"
#include "Units/Units.h"
#include "Units/UnitRoom.h"
#include "Drlg/D2DrlgDrlg.h"

// 1.13c D2Common+0x19650
extern "C" void d2_fn_d2common_19650(HD2ARCHIVE archive)
{
    // Compile two 0x34-byte token tables, concatenate, then release both inputs.
    DATATBLS_LoadPlrType_ModeTxt(archive);
}

// 1.13c D2Common+0x55400
extern "C" void d2_fn_d2common_55400(HD2ARCHIVE archive)
{
    // The sound-name linker is created only in text-compilation mode.
    DATATBLS_LoadSoundsTxt(archive);
}

// 1.13c D2Common+0x60920
extern "C" void d2_fn_d2common_60920(HD2ARCHIVE archive)
{
    D2BinFieldStrc fields[] = {
        {"Amazon", TXTFIELD_DWORD, 0, 0, nullptr},
        {"Sorceress", TXTFIELD_DWORD, 0, 4, nullptr},
        {"Necromancer", TXTFIELD_DWORD, 0, 8, nullptr},
        {"Paladin", TXTFIELD_DWORD, 0, 12, nullptr},
        {"Barbarian", TXTFIELD_DWORD, 0, 16, nullptr},
        {"Druid", TXTFIELD_DWORD, 0, 20, nullptr},
        {"Assassin", TXTFIELD_DWORD, 0, 24, nullptr},
        {"ExpRatio", TXTFIELD_DWORD, 0, 28, nullptr},
        {"end", TXTFIELD_NONE, 0, 0, nullptr}
    };
    static_assert(sizeof(D2ExperienceTxt) == 32);
    sgptDataTables->pExperienceTxt = static_cast<D2ExperienceDataTbl*>(
        DATATBLS_CompileTxt(archive, "experience", fields, nullptr, sizeof(D2ExperienceTxt)));
}

// 1.13c D2Common+0x616D0
extern "C" void d2_fn_d2common_616d0(HD2ARCHIVE archive)
{
    // compcode always loads; remaining code/name linkers require bCompileTxt.
    DATATBLS_LoadSomeTxts(archive);
}

// 1.13c D2Common+0x6CCF0
extern "C" void d2_fn_d2common_6ccf0(HD2ARCHIVE archive)
{
    // Resolves level/tile names, counts cels up to -1, builds contiguous ranges.
    DATATBLS_LoadAutomapTxt(archive);
}

// 1.13c D2Common+0x74400
extern "C" void d2_fn_d2common_74400(HD2ARCHIVE archive)
{
    DATATBLS_LoadLowQualityItemsTxt(archive);
}

// 1.13c D2Common+0x20DB0
extern "C" int32_t d2_fn_d2common_20db0(D2UnitStrc* player, int32_t slot,
    int32_t otherSlot, D2UnitStrc* incoming, D2UnitStrc* equipped,
    D2UnitStrc* otherItem, int32_t unused)
{
    // Same eight-way inventory decision tree as the 1.10f map.
    return sub_6FD91D50(player, slot, otherSlot, incoming, equipped, otherItem, unused);
}

// Native tile-library seams preserve wasm64 pointers; caller supplies the library
// and the act byte that retail loads from its owning room context.
using FindTiles = int32_t (*)(void*, int32_t, int32_t, int32_t, D2TileLibraryEntryStrc**, int32_t);
using TileFlags = uint32_t (*)(D2TileLibraryEntryStrc*);
static_assert(offsetof(D2DrlgTileDataStrc, dwFlags) == 0x14);
static_assert(offsetof(D2DrlgTileDataStrc, pTile) == 0x18);

// 1.13c D2Common+0x2E0C0
extern "C" void d2_fn_d2common_2e0c0(uint8_t act, void* library,
    D2DrlgTileDataStrc* output, FindTiles find, TileFlags flags)
{
    if (!output || !find || !flags) std::abort();
    std::memset(output, 0, sizeof(*output));
    int32_t style = 0, sequence = 0;
    if (act == 1) sequence = 1;
    else if (act == 2) { style = 29; sequence = 12; }
    else if (act != 0) return;
    D2TileLibraryEntryStrc* tiles[40]{};
    if (!find(library, 0, style, sequence, tiles, 40)) std::abort();
    output->pTile = tiles[0];
    output->nRed = output->nGreen = output->nBlue = 255;
    output->dwFlags = 0x4000;
    const uint32_t bits = flags(tiles[0]);
    if (bits & 1) output->dwFlags |= 4;
    if (bits & 4) output->dwFlags |= 0x800;
}

// 1.13c D2Common+0x34080
extern "C" void d2_fn_d2common_34080(const uint32_t* mask, D2UnitStrc* unit)
{
    if (!unit) return;
    uint32_t* gfx = D2COMMON_STATES_GetListGfxFlags_6FDB8AC0(unit);
    uint32_t* stats = D2COMMON_STATES_GetStatFlags_6FDB8A90(unit);
    if (!gfx || !stats) return;
    const int32_t words = (sgptDataTables->nStatesTxtRecordCount + 31) / 32;
    for (int32_t i = 0; i < words; ++i) {
        gfx[i] |= stats[i] & mask[i];
        stats[i] &= ~mask[i];
    }
    UNITROOM_RefreshUnit(unit);
}

// 1.13c D2Common+0x458C0
extern "C" uint8_t* d2_fn_d2common_458c0(uint8_t* base,
    uint32_t group, uint32_t index)
{
    if (group >= 3 || index >= 3) std::abort();
    return base + 0x30 + group * 0x30 + index * 0x10;
}

// 1.13c D2Common+0x35100
extern "C" uint32_t d2_fn_d2common_35100(D2UnitStrc* unit)
{
    // Retail assumes a valid monster record on this path; retain that invariant.
    D2MonStatsTxt* record = unit ? DATATBLS_GetMonStatsTxtRecord(unit->dwClassId) : nullptr;
    if (!record) std::abort();
    if (record->dwMonStatsFlags & MONSTATSFLAG_FLYING) return 0x1804;
    return (record->dwMonStatsFlags & MONSTATSFLAG_OPENDOORS) ? 0x3401 : 0x3c01;
}
