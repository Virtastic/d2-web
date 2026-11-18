// 1.13c D2Launch+0x167B0 (sub_167B0). Stack mode, ret 4. void. Calls sub_15D30, frees the first control table (thunk 0x9922 on each slot of
// DAT_6fa653c0, count DAT_6fa65e34 re-read per step), count := 0, loads the sky palette (sub_12BF0: EAX = 0x1e99c, stack 0x1e988), then appends
// eleven controls (thunk 0x9952) to the second table (DAT_6fa652b8, count DAT_6fa65e30) from the templates 0x210a0, 0x21a90, 0x21ac0, 0x21af0,
// 0x21b20, 0x21b50, 0x21b80, 0x21bb0, 0x21be0, 0x21c10, 0x21c40. The third gets a label: thunk 0x9B32 (ECX = 0x13fa) and thunk 0x98C8
// (ECX = control, EDX = that string). A Unicode local (one zero char) is built, then the controls 5..10 from the start the mode selects
// (mode 2..6 -> control mode+4, mode 7 -> none, anything else -> control 5) each get thunk 0x9862 (ECX = h, EDX = 0) then thunk 0x990A
// (ECX = h, EDX = 10, stack &local, 0). The 0x40 overflow exit (fatal box + _exit) is not reproduced.
#include <cstdint>

struct D2l167b0Seams {
    void (*f15d30)();
    void (*imp9922)(uint32_t* slot);
    void (*f12bf0)(uint32_t eax, uint32_t s0);
    uint32_t (*imp9952)(uint32_t tpl);
    uint32_t (*imp9b32)(uint32_t id);
    void (*imp98c8)(uint32_t h, uint32_t str);
    void (*imp9862)(uint32_t h, uint32_t z);
    void (*imp990a)(uint32_t h, uint32_t n, const uint16_t* text, uint32_t z);
};

extern "C" void d2l_167b0(int32_t mode, uint32_t* count1, uint32_t* table1, uint32_t* count2, uint32_t* table2, uint32_t modbase, const D2l167b0Seams* s) {
    s->f15d30();
    for (int32_t i = 0; i < (int32_t)*count1; i++) s->imp9922(&table1[i]);
    *count1 = 0;
    s->f12bf0(modbase + 0x1e99c, modbase + 0x1e988);
    static const uint32_t tpl[11] = { 0x210a0, 0x21a90, 0x21ac0, 0x21af0, 0x21b20, 0x21b50, 0x21b80, 0x21bb0, 0x21be0, 0x21c10, 0x21c40 };
    uint32_t h[11];
    for (int i = 0; i < 11; i++) {
        h[i] = s->imp9952(modbase + tpl[i]);
        table2[(*count2)++] = h[i];
        if (i == 2) s->imp98c8(h[2], s->imp9b32(0x13fa));
    }
    uint16_t text[1] = { 0 };
    int first = (mode >= 2 && mode <= 7) ? mode + 3 : 4;   // index into h: control 5 is h[4]
    for (int i = first; i <= 9; i++) {
        s->imp9862(h[i], 0);
        s->imp990a(h[i], 10, text, 0);
    }
}
