// 1.13c D2Launch+0x15090 (sub_15090). Stack a1 (text pointer), a2 (stored in DAT_6fa6552c), a3 (selects the second control's template), ret 0xC.
// Same teardown as sub_14C60, then builds three controls (thunk 0x9952): with DAT_6fa65ad8 != 0 the templates 0x23d70, record 0xF8 + 2*(a3 == 0)
// of the table at 0x20f20 (0x30 bytes each), 0x23dd0; else 0x238c0, record 0xDF + 2*(a3 != 0), 0x23920. The first is flagged (thunk 0x995E: h, 1),
// all three are shown (0x98B0: h, 1) and the third gets the text a1 through sub_12A70 (h3, a1, 0; BL cleared by the caller). The 0x40 overflow exit is not reproduced.
#include <cstdint>

extern "C" void d2l_15090(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t* count, uint32_t* table, const uint32_t* flag, uint32_t* g6552c,
                          uint32_t modbase, void (*imp9922)(uint32_t*), uint32_t (*imp9952)(uint32_t), void (*imp995e)(uint32_t, uint32_t),
                          void (*imp98b0)(uint32_t, uint32_t), void (*imp12a70)(uint32_t, uint32_t, uint32_t)) {
    int32_t n = (int32_t)*count;
    for (int32_t i = 0; i < n; i++) imp9922(&table[i]);
    *count = 0;
    uint32_t ra, rec, rc;
    if (*flag != 0) { ra = 0x23d70; rec = 0xf8u + 2u * (a3 == 0); rc = 0x23dd0; }
    else { ra = 0x238c0; rec = 0xdfu + 2u * (a3 != 0); rc = 0x23920; }
    uint32_t h1 = imp9952(modbase + ra);
    table[(*count)++] = h1;
    imp995e(h1, 1);
    uint32_t h2 = imp9952(modbase + 0x20f20u + rec * 0x30u);
    table[(*count)++] = h2;
    uint32_t h3 = imp9952(modbase + rc);
    table[(*count)++] = h3;
    imp98b0(h1, 1);
    imp98b0(h2, 1);
    imp98b0(h3, 1);
    imp12a70(h3, a1, 0);
    *g6552c = a2;
}
