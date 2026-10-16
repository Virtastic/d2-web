// 1.13c D2Common+0x22F80 / +0x23590 / +0x235D0 / +0x23610 / +0x33B00: bounds-checked single-field readers
//
#include <cstddef>
#include <cstdint>
#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"
static_assert(sizeof(D2ItemTypesTxt) == 0xE4, "itemtypes record");
static_assert(offsetof(D2ItemTypesTxt, wShoots) == 0xC, "+0xC");
static_assert(offsetof(D2ItemTypesTxt, wQuiver) == 0xE, "+0xE");
static_assert(offsetof(D2ItemTypesTxt, nThrowable) == 0x10, "+0x10");
static_assert(offsetof(D2ItemTypesTxt, nAutoStack) == 0x13, "+0x13");
static const D2ItemTypesTxt* Rec(int32_t id)
{
    return (id >= 0 && id < sgptDataTables->nItemTypesTxtRecordCount) ? &sgptDataTables->pItemTypesTxt[id] : nullptr;
}
uint32_t D2Common_22F80_ItemTypeThrowable(int32_t id) { const D2ItemTypesTxt* r = Rec(id); return r ? r->nThrowable : 0; }
uint32_t D2Common_23590_ItemTypeAutoStack(int32_t id) { const D2ItemTypesTxt* r = Rec(id); return r ? r->nAutoStack : 0; }
int32_t D2Common_235D0_ItemTypeQuiver(int32_t id) { const D2ItemTypesTxt* r = Rec(id); return r ? (int16_t)r->wQuiver : 0; }
int32_t D2Common_23610_ItemTypeShoots(int32_t id) { const D2ItemTypesTxt* r = Rec(id); return r ? (int16_t)r->wShoots : 0; }
uint32_t D2Common_33B00_StateMask12Bit(int32_t id)
{
    return (id >= 0 && id < sgptDataTables->nStatesTxtRecordCount) ? (sgptDataTables->fStateMasks[12][id >> 5] & (1u << (id & 31))) : 0;
}
