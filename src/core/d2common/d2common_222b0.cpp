// 1.13c D2Common+0x222b0: items.txt nCompactSave byte of an item's class.
// Entry: EAX = pItem; the class id is the dword at +4. Retail's out-of-range and no-table paths call _exit(-1);
// this port returns 0 for them and the capture never reaches them.
#include <cstddef>
#include <cstdint>
#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"
#include "Units/Units.h"

static_assert(sizeof(D2ItemsTxt) == 0x1A8, "items.txt record stride");
static_assert(offsetof(D2ItemsTxt, nCompactSave) == 0x143, "items.txt nCompactSave at +0x143");

uint8_t D2Common_222B0(const D2UnitStrc* pItem) {
    const uint32_t nClass = pItem ? pItem->dwClassId : 0xFFFFFFFFu;
    const D2ItemDataTbl& t = sgptDataTables->pItemDataTables;
    if (nClass < (uint32_t)t.nItemsTxtRecordCount && t.pItemsTxt) {
        return t.pItemsTxt[nClass].nCompactSave;
    }
    return 0;
}
