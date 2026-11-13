// 1.13c D2Client+0x82210, on real units.
#include "d2client_85990.h"
#include <D2StatList.h>
#include <D2Inventory.h>
#include <D2DataTbls.h>
#include <Units/Units.h>

int d2_client_charge_set_82210(D2UnitStrc* unit, int32_t p2, uint32_t p3, int32_t p4) {
    const int32_t key = (p2 << (sgptDataTables->nStuff & 0x1f)) + (sgptDataTables->nShiftedStuff & p3);
    D2StatListStrc* list = STATLIST_GetStatListFromUnitAndFlag(unit, 0x40);
    int32_t held = 0;
    for (; list; list = STATLIST_GetStatListFromFlag(list, 0x40))
        if ((held = STATLIST_GetStatValue(list, 0xcc, static_cast<uint16_t>(key))) != 0) break;
    if (!list) {
        if (unit->pInventory)
            for (D2UnitStrc* it = INVENTORY_GetFirstItem(unit->pInventory); it; it = INVENTORY_GetNextItem(it))
                if (d2_client_charge_set_82210(INVENTORY_UnitIsItem(it), p2, p3, p4)) return 1;
        return 0;
    }
    const int32_t max = held >> 8;
    if (max < 1 || max > 0xff) return 0;
    int32_t now = p4 < 1 ? 0 : p4;
    if (now >= max) now = max;
    STATLIST_SetBaseStat(list, 0xcc, max * 0x100 + (now & 0xff), static_cast<uint16_t>(key), unit);
    return 1;
}
