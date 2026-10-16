// 1.13c D2Common+0x227C0 (#10349)
#include <cstdint>
#include <cstddef>
#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"
static_assert(offsetof(D2ItemsTxt, dwBitField1) == 0xDC, "items.txt dwBitField1");
// Bit 0 of items.txt row nItemId's dwBitField1 (+0xDC). Out of range is fatal in retail (_exit); we return 0.
uint32_t D2Common_227C0_ItemBitField1Bit0(uint32_t nItemId)
{
    const D2ItemsTxt* p = DATATBLS_GetItemsTxtRecord((int)nItemId);
    return p ? p->dwBitField1 & 1 : 0;
}
