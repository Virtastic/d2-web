// 1.13c D2Launch+0x15400 (sub_15400). Stack a1, a2, a3 (UTF-16 text pointer), a4, ret 0x10. Teardown as sub_14C60, then builds controls (thunk 0x9952)
// from the templates 0x23b30 (flagged by thunk 0x995E: h, 1), 0x23b60, 0x23b90; shows each (0x98B0: h, 1); the third gets sub_12A70 (h, a1, 0).
// With a3 non-null and *a3 != 0: sub_12BA0 with control records 0xEF then 0xF0 (shown); record 0xEF gets (h, a2, 0), record 0xF0 gets (h, a3, 4).
// Else: control template 0x23bc0, shown, with (h, a2, 0). DAT_6fa6552c = a4. The 0x40 overflow exit is not reproduced.
#include <cstdint>

extern "C" uint32_t d2l_12ba0(uint32_t eax, uint32_t base, uint32_t* count, uint32_t* table, uint32_t (*imp10098)(uint32_t));

extern "C" void d2l_15400(uint32_t a1, uint32_t a2, uint32_t a3, const uint16_t* a3text, uint32_t a4, uint32_t* count, uint32_t* table,
                          uint32_t* g6552c, uint32_t modbase, void (*imp9922)(uint32_t*), uint32_t (*imp9952)(uint32_t),
                          void (*imp995e)(uint32_t, uint32_t), void (*imp98b0)(uint32_t, uint32_t),
                          void (*imp12a70)(uint32_t, uint32_t, uint32_t)) {
    int32_t n = (int32_t)*count;
    for (int32_t i = 0; i < n; i++) imp9922(&table[i]);
    *count = 0;
    uint32_t h = imp9952(modbase + 0x23b30);
    table[(*count)++] = h;
    imp995e(h, 1);
    imp98b0(h, 1);
    h = imp9952(modbase + 0x23b60);
    table[(*count)++] = h;
    imp98b0(h, 1);
    h = imp9952(modbase + 0x23b90);
    table[(*count)++] = h;
    imp98b0(h, 1);
    imp12a70(h, a1, 0);
    if (a3text && *a3text) {
        h = d2l_12ba0(0xef, modbase + 0x20f20, count, table, imp9952);
        imp98b0(h, 1);
        imp12a70(h, a2, 0);
        h = d2l_12ba0(0xf0, modbase + 0x20f20, count, table, imp9952);
        imp98b0(h, 1);
        imp12a70(h, a3, 4);
    } else {
        h = imp9952(modbase + 0x23bc0);
        table[(*count)++] = h;
        imp98b0(h, 1);
        imp12a70(h, a2, 0);
    }
    *g6552c = a4;
}
