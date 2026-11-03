#include <cstdint>
// 1.13c D2Launch+0x1B310 -- sky-background UI: clear the screen list (0x15D30), push three templates through D2Win#10098 onto the
// 0x40-slot table at 0x6fa652b8 (count at 0x6fa65e30; a full table is fatal, error 0x624), load the sky palette with 0x12BF0
// (stack = the palette name, EAX = the second string, ret 4) and tail-jump to 0x110C0.
struct D2l1b310Seams {
    void (*f15d30)();
    uint32_t (*win10098)(uint32_t tmpl);
    void (*f12bf0)(uint32_t eax, uint32_t name);
    void (*f110c0)();
    void (*fatal)(uint32_t id);
};
extern "C" void d2l_1b310_core(int32_t* count, uint32_t* table, const uint32_t tmpl[3], uint32_t palName, uint32_t palEax,
                               const D2l1b310Seams* s) {
    s->f15d30();
    for (int i = 0; i < 3; ++i) {
        if (*count >= 0x40) { s->fatal(0x624); return; }
        uint32_t h = s->win10098(tmpl[i]);
        table[*count] = h;
        *count += 1;
    }
    s->f12bf0(palEax, palName);
    s->f110c0();
}
