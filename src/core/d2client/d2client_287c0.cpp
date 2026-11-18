// 1.13c D2Client+0x287C0: the low-durability / low-quantity warning scan for the player at global 0x11BBFC. The six warning
// slot bytes at 0x11C488 are cleared. The two hand slots (#C262(inventory, 4 / 5)) raise warning 2 when one holds an item
// whose #C772 ammo kind the other side cannot take (#C1CC) or that side is empty. Then every body location listed at 0xF53A4
// (count at 0xF53AC) holding an item with a nonzero #CF3A warning class is checked against the five-word records at 0xF53B0
// (count at 0xF5414): {class, warning, limit, limit for +1, limit for +2}. For a matching record the item's
// stat 0x46 (#C1C6) is summed, adding every other inventory item (#C4F0 / #C526 / #C568 walk, skipping the item itself and
// #C92E ones) whose #C1BA record has the same +0x80 code, unless #CF40 marks the item. The sum then picks warning+2 (<= rec[4]),
// warning+1 (<= rec[3]) or warning (<= rec[2]) for 0x28530; above rec[2] nothing.
#include "d2client_callee.h"
// 1.13c D2Client+0x287C0
extern "C" void d2c_287c0(const D2cSvc& v, const uint32_t* player /*[0x11BBFC]*/, uint8_t* slots /*0x11C488, 6 bytes*/,
                          const uint32_t* tab /*0xF53A4: [0..] locations, [2] count; [3..27] records; [28] count*/) {
    const uint32_t inv = player[0x60 / 4];
    for (int i = 0; i < 6; ++i) slots[i] = 0;
    const uint32_t a = d2c_call(v, 0xc262, 0, 0, 0, 0, 0, inv, 4);
    const uint32_t b = d2c_call(v, 0xc262, 0, 0, 0, 0, 0, inv, 5);
    bool warn = false;
    if (a && d2c_call(v, 0xc772, 0, 0, 0, 0, 0, a))
        warn = !b || !d2c_call(v, 0xc1cc, 0, 0, 0, 0, 0, b, d2c_call(v, 0xc772, 0, 0, 0, 0, 0, a));
    if (!warn && b && d2c_call(v, 0xc772, 0, 0, 0, 0, 0, b))
        warn = !a || !d2c_call(v, 0xc1cc, 0, 0, 0, 0, 0, a, d2c_call(v, 0xc772, 0, 0, 0, 0, 0, b));
    if (warn) d2c_call(v, 0x28530, 0, 0, 0, 0, 0, 2);
    for (uint32_t i = 0; i < tab[2]; ++i) {
        const uint32_t* it = (const uint32_t*)(uintptr_t)d2c_call(v, 0xc262, 0, 0, 0, 0, 0, inv, tab[i]);
        if (!it) continue;
        const uint32_t itu = (uint32_t)(uintptr_t)it;
        const uint32_t cls = d2c_call(v, 0xcf3a, 0, 0, 0, 0, 0, itu) & 0xff;
        if (!cls) continue;
        for (uint32_t j = 0; j < tab[28]; ++j) {
            const uint32_t* rec = tab + 3 + 5 * j;
            if (cls != rec[0]) continue;
            const uint32_t* txt = (const uint32_t*)(uintptr_t)d2c_call(v, 0xc1ba, 0, 0, 0, 0, 0, it[1]);
            uint32_t sum = d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, itu, 0x46, 0);
            if (!d2c_call(v, 0xcf40, 0, 0, 0, 0, 0, itu)) {
                for (uint32_t n = d2c_call(v, 0xc4f0, 0, 0, 0, 0, 0, inv); n; n = d2c_call(v, 0xc568, 0, 0, 0, 0, 0, n)) {
                    const uint32_t e = d2c_call(v, 0xc526, 0, 0, 0, 0, 0, n);
                    if (e == itu || (d2c_call(v, 0xc92e, 0, 0, 0, 0, 0, e) & 0xff)) continue;
                    const uint32_t* t2 = (const uint32_t*)(uintptr_t)d2c_call(v, 0xc1ba, 0, 0, 0, 0, 0,
                                                                            e ? ((const uint32_t*)(uintptr_t)e)[1] : 0xffffffffu);
                    if (t2[0x80 / 4] == txt[0x80 / 4]) sum += d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, e, 0x46, 0);
                }
            }
            if (sum <= rec[4]) d2c_call(v, 0x28530, 0, 0, 0, 0, 0, rec[1] + 2);
            else if (sum <= rec[3]) d2c_call(v, 0x28530, 0, 0, 0, 0, 0, rec[1] + 1);
            else if (sum <= rec[2]) d2c_call(v, 0x28530, 0, 0, 0, 0, 0, rec[1]);
        }
    }
}
