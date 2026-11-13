// 1.13c D2Client+0x85990, after the unit lookup the caller does.
// The stat is written to the item only when the packet's flag byte is 1; stat 0xCC (charged skill) never goes through
// the plain write but rewrites a charge count (0x82210); with a player present any other stat refreshes the player's
// equipment (0x82FE0, not bound to real units yet) and a quantity (0x46) above zero clears the item's targeting and no-equip flags.
#include "d2client_85990.h"
#include <D2Items.h>
#include <D2StatList.h>
#include <D2DataTbls.h>
#include <Units/Units.h>

void d2_client_item_stat_85990(D2UnitStrc* hero, D2UnitStrc* item, int32_t count, char flag, uint16_t arg, int32_t cmd) {
    if (!item) return;
    if (cmd == 0xcc) {
        if (hero) d2_client_charge_set_82210(item, arg >> (sgptDataTables->nStuff & 0x1f), sgptDataTables->nShiftedStuff & arg, count & 0xff);
        return;
    }
    if (flag == 1) STATLIST_SetUnitStat(item, cmd, count, 0);
    if (!hero || cmd != 0x46 || count <= 0) return;
    if (ITEMS_CheckItemFlag(item, IFLAG_TARGETING, __LINE__, __FILE__)) ITEMS_SetItemFlag(item, IFLAG_TARGETING, 0);
    if (ITEMS_CheckItemFlag(item, IFLAG_NOEQUIP, __LINE__, __FILE__)) ITEMS_SetItemFlag(item, IFLAG_NOEQUIP, 0);
}
