// 1.13c D2Launch+0x15230 (sub_15230). Stack a1..a5, ret 0x14. Teardown as sub_14C60, then builds six controls (thunk 0x9952) from the templates
// 0x23c50, 0x23c80, 0x23cb0, 0x23ce0, 0x23d10, 0x23d40, appending each to the table. The first gets thunk 0x995E (h, 1); every control is shown
// (thunk 0x98B0: h, 1); controls 3..6 get text through sub_12A70 (h, a1, 0), (h, a2, 0), (h, a3, 0), (h, a4, 4). DAT_6fa6552c = a5. The 0x40 overflow exit is not reproduced.
#include <cstdint>

extern "C" void d2l_15230(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t* count, uint32_t* table, uint32_t* g6552c,
                          uint32_t modbase, void (*imp9922)(uint32_t*), uint32_t (*imp9952)(uint32_t), void (*imp995e)(uint32_t, uint32_t),
                          void (*imp98b0)(uint32_t, uint32_t), void (*imp12a70)(uint32_t, uint32_t, uint32_t)) {
    int32_t n = (int32_t)*count;
    for (int32_t i = 0; i < n; i++) imp9922(&table[i]);
    *count = 0;
    uint32_t h = imp9952(modbase + 0x23c50);
    table[(*count)++] = h;
    imp995e(h, 1);
    imp98b0(h, 1);
    h = imp9952(modbase + 0x23c80);
    table[(*count)++] = h;
    imp98b0(h, 1);
    static const uint32_t tpl[4] = { 0x23cb0, 0x23ce0, 0x23d10, 0x23d40 };
    const uint32_t txt[4] = { a1, a2, a3, a4 };
    for (int i = 0; i < 4; i++) {
        h = imp9952(modbase + tpl[i]);
        table[(*count)++] = h;
        imp98b0(h, 1);
        imp12a70(h, txt[i], i == 3 ? 4u : 0u);
    }
    *g6552c = a5;
}
