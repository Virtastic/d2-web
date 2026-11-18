// 1.13c D2Common+0x29F30 over the engine: the port's callees as the vendored table and stat functions.
// ITEMS_SetDefenseOrDamage (the play-path callers in Items.cpp) is this port; no second copy of the body remains.
#include <cstdint>
#include "D2Items.h"
#include "D2StatList.h"
#include "D2DataTbls.h"
#include "D2Common_29F30.h"

static_assert(sizeof(D2ItemsTxt) == 0x1a8, "retail weapons/armor row stride");

static uint32_t e_rd32(uintptr_t a) { return *reinterpret_cast<const uint32_t*>(a); }
static uint32_t e_rd8(uintptr_t a) { return *reinterpret_cast<const uint8_t*>(a); }
static uintptr_t e_itemTxt(uint32_t cls) { return reinterpret_cast<uintptr_t>(DATATBLS_GetItemsTxtRecord(cls)); }
static int32_t e_codeIndex(uint32_t code) { return DATATBLS_GetItemIdFromItemCode(code); }
static uintptr_t e_row(int32_t i) { return reinterpret_cast<uintptr_t>(&sgptDataTables->pItemDataTables.pItemsTxt[i]); }
static uint32_t e_typeIs(uintptr_t it, uint32_t t) { return ITEMS_CheckItemTypeId(reinterpret_cast<D2UnitStrc*>(it), (int)t); }
static int32_t e_getBase(uintptr_t it, int32_t s) { return STATLIST_GetUnitBaseStat(reinterpret_cast<D2UnitStrc*>(it), s, 0); }
static uint32_t e_throwable(uintptr_t it) { return ITEMS_CheckIfThrowable(reinterpret_cast<D2UnitStrc*>(it)); }
static void e_setStat(uintptr_t it, int32_t s, int32_t v) { STATLIST_SetUnitStat(reinterpret_cast<D2UnitStrc*>(it), s, v, 0); }

extern "C" void D2Common_29F30_ItemDefenseOrDamageLive(D2UnitStrc* item, int32_t stat)
{
    static const D2Common29F30Env env = { e_rd32, e_rd8, e_itemTxt, e_codeIndex, e_row, e_typeIs, e_getBase, e_throwable, e_setStat };
    D2Common_29F30_ItemDefenseOrDamage(reinterpret_cast<uintptr_t>(item), (uint32_t)stat, &env);
}

void __fastcall ITEMS_SetDefenseOrDamage(D2UnitStrc* pItem, int nStat)
{
    D2Common_29F30_ItemDefenseOrDamageLive(pItem, nStat);
}
