// D2Common 1.13c batch: items.txt row from a unit or a fallback record.
#include <cstdint>
#include <emscripten.h>
#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"

// 1.13c D2Common+0x45A40: pUnit (EAX) a UNIT_ITEM (+0x00 == 4) -> items.txt row of its class id (+0x04).
// Otherwise pRec (ECX) null -> index -1, else pRec+4; an index past the table gives null.
// Both routes read the same 0x1A8-stride table; retail's fatal exit on a missing table is not modelled.
extern "C" EMSCRIPTEN_KEEPALIVE const D2ItemsTxt* D2Common_45A40_ItemsRecord(const int32_t* pUnit, const int32_t* pRec)
{
    if (pUnit != nullptr && pUnit[0] == 4)
        return DATATBLS_GetItemsTxtRecord(pUnit[1]);
    return DATATBLS_GetItemsTxtRecord(pRec == nullptr ? -1 : pRec[1]);
}
