// 1.13c D2Launch+0x12070 (sub_12070). No arguments, returns 1. DAT_6fa65e74 points at an array of DAT_6fa65e78 entry pointers. If it is null,
// nothing happens. For each non-null entry e (two dwords): thunk 0x97FC (D2Lang#10043, ECX = value, two stack words id, 0) is called with
// (e[1], 0x1121), (e[0], 0x1123), (e, 0x1125); a zero e[1] or e[0] is a fatal box + _exit (not reproduced). Last, (table, 0x1128) and DAT_6fa65e74 := 0.
// rd reads the dword at an address (the entries are 4-byte retail cells).
#include <cstdint>

extern "C" uint32_t d2l_12070(uint32_t* pTable, const int32_t* pCount, uint32_t (*rd)(uint32_t), void (*imp97fc)(uint32_t, uint32_t, uint32_t)) {
    uint32_t t = *pTable;
    if (!t) return 1;
    for (int32_t i = 0; i < *pCount; i++) {
        uint32_t e = rd(t + 4u * (uint32_t)i);
        if (!e) continue;
        if (!rd(e + 4)) return 1;
        imp97fc(rd(e + 4), 0x1121, 0);
        if (!rd(e)) return 1;
        imp97fc(rd(e), 0x1123, 0);
        imp97fc(e, 0x1125, 0);
    }
    imp97fc(t, 0x1128, 0);
    *pTable = 0;
    return 1;
}
