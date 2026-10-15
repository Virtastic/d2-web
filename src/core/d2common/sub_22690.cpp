// 1.13c D2Common+0x22690 (#10318)
#include <cstdint>
#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"
// 1 when items.txt row nItemId is not a quest item and its first type is not 0x27, else 0. Out of range is fatal in retail (_exit); we return 0.
uint32_t D2Common_22690_ItemPlain(uint32_t nItemId)
{
    const D2ItemsTxt* p = DATATBLS_GetItemsTxtRecord((int)nItemId);
    return p && !p->nQuest && p->wType[0] != 0x27;
}
