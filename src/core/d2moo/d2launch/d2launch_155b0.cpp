// 1.13c D2Launch+0x155B0 (sub_155B0). Stack a1..a3, ret 0xC. void. Frees the control table (D2Win#10050 via thunk 0x9922 on each slot), count := 0,
// then builds four controls (thunk 0x9952) from the templates 0x23a70, 0x23aa0, 0x23ad0, 0x23b00, appending each to the table. The first gets
// thunk 0x995E (h, 1); every control is shown (thunk 0x98B0: h, 1); controls 3 and 4 get text through sub_12A70 (h, a1, 0) and (h, a2, 0).
// DAT_6fa6552c := a3. The 0x40 overflow exit is unreachable (the table was just emptied) and is not reproduced.
#include <cstdint>

extern "C" void d2l_155b0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t* count, uint32_t* table, uint32_t* g6552c, uint32_t modbase,
                          void (*imp9922)(uint32_t*), uint32_t (*imp9952)(uint32_t), void (*imp995e)(uint32_t, uint32_t),
                          void (*imp98b0)(uint32_t, uint32_t), void (*imp12a70)(uint32_t, uint32_t, uint32_t)) {
    for (int32_t i = 0; i < (int32_t)*count; i++) imp9922(&table[i]);
    *count = 0;
    uint32_t h = imp9952(modbase + 0x23a70);
    table[(*count)++] = h;
    imp995e(h, 1);
    imp98b0(h, 1);
    h = imp9952(modbase + 0x23aa0);
    table[(*count)++] = h;
    imp98b0(h, 1);
    h = imp9952(modbase + 0x23ad0);
    table[(*count)++] = h;
    imp98b0(h, 1);
    imp12a70(h, a1, 0);
    h = imp9952(modbase + 0x23b00);
    table[(*count)++] = h;
    imp98b0(h, 1);
    imp12a70(h, a2, 0);
    *g6552c = a3;
}
