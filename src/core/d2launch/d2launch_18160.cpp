#include <cstdint>
// 1.13c D2Launch+0x18160 -- title/legal screen background: clear the screen list (0x15D30), push four templates onto the 0x40-slot
// table at 0x6fa652b8 (count at 0x6fa65e30) through D2Win#10098, load the sky palette (0x12BF0: stack = name, EAX = the second
// string), check the table again, push the fifth template, label it with two D2Lang#10003 strings (ECX = id, no stack) through D2Win#10000(ecx=handle,
// edx=string, stack 4,0,0) (the first string id is 0x557A when Fog#10227 is non-zero, else 0x13F0; the second 0x13F1), D2Win#10149(0) and, when
// D2sound#10035 is non-zero, tail-jump to D2sound#10045 with ECX = 1. A full table is fatal (error 0x624).
struct D2l18160Seams {
    void (*f15d30)();
    uint32_t (*win10098)(uint32_t tmpl);
    void (*f12bf0)(uint32_t eax, uint32_t name);
    void (*fatal)(uint32_t id);
    int (*fog10227)();
    uint32_t (*lang10003)(uint32_t ecx);
    void (*win10000)(uint32_t ecx, uint32_t edx);  // stack: 4, 0, 0 (ret 0xc)
    void (*win10149)(uint32_t z);
    int (*snd10035)();
    void (*snd10045)(uint32_t ecx);
};
extern "C" void d2l_18160_core(int32_t* count, uint32_t* table, const uint32_t tmpl[5], uint32_t palName, uint32_t palEax,
                               const D2l18160Seams* s) {
    s->f15d30();
    for (int i = 0; i < 4; ++i) {
        if (*count >= 0x40) { s->fatal(0x624); return; }
        uint32_t h = s->win10098(tmpl[i]);
        table[*count] = h;
        *count += 1;
    }
    s->f12bf0(palEax, palName);
    if (*count >= 0x40) { s->fatal(0x624); return; }
    uint32_t h = s->win10098(tmpl[4]);
    table[*count] = h;
    *count += 1;
    uint32_t t = s->lang10003(s->fog10227() ? 0x557a : 0x13f0);
    s->win10000(h, t);
    t = s->lang10003(0x13f1);
    s->win10000(h, t);
    s->win10149(0);
    if (s->snd10035()) s->snd10045(1);
}
