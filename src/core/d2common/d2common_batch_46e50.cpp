// D2Common 1.13c +0x46E50: item-unit stat application by item-type class (unit passed in ESI by retail).
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "DataTbls/ItemsTbls.h"
#include "Fog.h"

extern "C" uint8_t D2Common_ItemIsThrowable_24C30(uint32_t pItem);
extern "C" int32_t d2_fn_d2common_38c20(D2UnitStrc* pUnit, int32_t nStatId, int32_t nLayer);

// DAT_6fdeff6c = sgptDataTables->pItemsLinker (FOG_10215 code linker, Ordinal_10213 = FOG_GetLinkIndex);
// DAT_6fdefb98 = sgptDataTables->pItemDataTables.pItemsTxt (0x1a8-stride items.txt rows).
extern "C" {
// 1.13c D2Common+0x46E50
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_46e50(D2UnitStrc* pu) {
    const int32_t* u = reinterpret_cast<const int32_t*>(pu);
    uint32_t msg;
    if (!u) msg = 0x61b;
    else if (u[0] == 4) {
        const uint8_t* it = reinterpret_cast<const uint8_t*>(DATATBLS_GetItemsTxtRecord(u[1]));
        if (it) {
            const uint32_t key = *reinterpret_cast<const uint32_t*>(it + 0x80);
            const int32_t idx = FOG_GetLinkIndex(sgptDataTables->pItemsLinker, key, 0);
            if (idx >= 0) {
                const uint8_t* r = reinterpret_cast<const uint8_t*>(sgptDataTables->pItemDataTables.pItemsTxt) + static_cast<int64_t>(idx) * 0x1a8;
                if (r) switch (key) {
                case 0x10: case 0x1f:
                    if (ITEMS_CheckItemTypeId(pu, 0x32)) {
                        const int32_t lim = *reinterpret_cast<const int32_t*>(r + 0xd0);
                        if (lim) {
                            int32_t v = d2_fn_d2common_38c20(pu, 0x1f, 0) + 1;
                            if (v <= lim) v = lim + 1;
                            STATLIST_SetUnitStat(pu, 0x1f, v, 0);
                        }
                    }
                    break;
                case 0x11: case 0x16:
                    if (ITEMS_CheckItemTypeId(pu, 0x2d)) {
                        if (r[0xff]) STATLIST_SetUnitStat(pu, 0x16, r[0xff], 0);
                        if (r[0x103]) STATLIST_SetUnitStat(pu, 0x18, r[0x103], 0);
                        if ((D2Common_ItemIsThrowable_24C30(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pu))) & 0xff) && r[0x101]) STATLIST_SetUnitStat(pu, 0xa0, r[0x101], 0);
                    }
                    break;
                case 0x12: case 0x15:
                    if (ITEMS_CheckItemTypeId(pu, 0x2d)) {
                        if (r[0xfe]) STATLIST_SetUnitStat(pu, 0x15, r[0xfe], 0);
                        if (r[0x102]) STATLIST_SetUnitStat(pu, 0x17, r[0x102], 0);
                        if ((D2Common_ItemIsThrowable_24C30(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pu))) & 0xff) && r[0x100]) STATLIST_SetUnitStat(pu, 0x9f, r[0x100], 0);
                    }
                    break;
                }
            }
            return;
        }
        msg = 0x61f;
    } else msg = 0x61c;
    (void)msg;
    __builtin_trap(); // retail: Ordinal_10265/10024 report then _exit(-1)
}
}
