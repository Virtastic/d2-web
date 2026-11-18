// 1.13c D2Launch+0x14C60 (sub_14C60). No arguments. Destroys every control in the screen table (D2Win#10050, thunk 0x9922, one
// pointer to the table cell), clears the count, sets DAT_6fa65e40 = 1 and DAT_6fa65e60 = 0, builds three controls (D2Win#10098,
// thunk 0x9952, from the templates 0x23d70/0x23e00/0x23dd0 when DAT_6fa65ad8 != 0, else 0x238c0/0x238f0/0x23920), appends each to the
// table, sets the first one's flag (#10196, thunk 0x995E: h, 0), shows all three (#10001, thunk 0x98B0: h, 1), looks up string 0x147B
// (D2Lang#10003, thunk 0x9B32, ECX) and hands it to the third control (thunk 0x98C8: ECX = handle, EDX = text). Finally
// DAT_6fa6552c = &sub_12A60. The 0x40-entry overflow exit is not reproduced. modbase: the address the module's RVAs are added to.
#include <cstdint>

extern "C" void d2l_14c60(uint32_t* count, uint32_t* table, const uint32_t* flag, uint32_t* g65e40, uint32_t* g65e60, uint32_t* g6552c,
                          uint32_t modbase, void (*imp9922)(uint32_t*), uint32_t (*imp9952)(uint32_t), void (*imp995e)(uint32_t, uint32_t),
                          void (*imp98b0)(uint32_t, uint32_t), uint32_t (*imp9b32)(uint32_t), void (*imp98c8)(uint32_t, uint32_t)) {
    int32_t n = (int32_t)*count;
    for (int32_t i = 0; i < n; i++) imp9922(&table[i]);
    *count = 0;
    *g65e40 = 1;
    *g65e60 = 0;
    uint32_t ra, rb, rc;
    if (*flag != 0) { ra = 0x23d70; rb = 0x23e00; rc = 0x23dd0; }
    else { ra = 0x238c0; rb = 0x238f0; rc = 0x23920; }
    uint32_t h1 = imp9952(modbase + ra);
    table[(*count)++] = h1;
    imp995e(h1, 0);
    uint32_t h2 = imp9952(modbase + rb);
    table[(*count)++] = h2;
    uint32_t h3 = imp9952(modbase + rc);
    table[(*count)++] = h3;
    imp98b0(h1, 1);
    imp98b0(h2, 1);
    imp98b0(h3, 1);
    imp98c8(h3, imp9b32(0x147b));
    *g6552c = modbase + 0x12a60;
}
