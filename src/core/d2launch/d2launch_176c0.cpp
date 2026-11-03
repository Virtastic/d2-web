#include <cstdint>
// 1.13c D2Launch+0x176C0 -- sky-palette screen resources: clear the screen list (0x15D30), push three templates through
// D2Win#10098 onto the 0x40-slot table at 0x6fa652b8 (count at 0x6fa65e30; a full table is fatal, error 0x624); after the first
// two, D2Win#10091(handle, 0) hides the control; then load the sky palette with 0x12BF0 (stack = name, EAX = the second string).
struct D2l176c0Seams {
    void (*f15d30)();
    uint32_t (*win10098)(uint32_t tmpl);
    void (*win10091)(uint32_t h, uint32_t z);
    void (*f12bf0)(uint32_t eax, uint32_t name);
    void (*fatal)(uint32_t id);
};
extern "C" void d2l_176c0_core(int32_t* count, uint32_t* table, const uint32_t tmpl[3], uint32_t palName, uint32_t palEax,
                               const D2l176c0Seams* s) {
    s->f15d30();
    for (int i = 0; i < 3; ++i) {
        if (*count >= 0x40) { s->fatal(0x624); return; }
        uint32_t h = s->win10098(tmpl[i]);
        table[*count] = h;
        *count += 1;
        if (i < 2) s->win10091(h, 0);
    }
    s->f12bf0(palEax, palName);
}
