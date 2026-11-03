#include <cstdint>
// 1.13c D2Launch+0x182D0 -- progress-bar screen: 0x1B310 first, then six templates pushed onto the 0x40-slot table (a full table is
// fatal, error 0x624 -- checked before every push). 2: stored in g25E68 and D2Win#10174(h, 3); 3: stored in g25E64; then the percent
// comes from vtable slot +0x4C (clamped above at 100); 4 and 5: D2Lang string 0x143A / 0x143B through D2Win#10000(handle, string,
// 0, 0, 0); 6: the bar fill D2Win#10161(g25E68, D2Win#10112(g25E68) * pct / 100), then sprintf(buf, "%d%%", pct) and
// D2Win#10022(g25E64, buf, 0).
struct D2l182d0Globals { int32_t* count; uint32_t *g25e68, *g25e64; uint32_t* table; };
struct D2l182d0Seams {
    void (*f1b310)();
    uint32_t (*win10098)(uint32_t tmpl);
    void (*win10174)(uint32_t h, uint32_t three);
    int32_t (*vt4c)();
    uint32_t (*lang10003)(uint32_t id);
    void (*win10000)(uint32_t ecx, uint32_t edx);
    int32_t (*win10112)(uint32_t h);
    void (*win10161)(uint32_t h, int32_t v);
    void (*sprintf3)(char* buf, uint32_t fmt, int32_t pct);
    void (*win10022)(uint32_t ecx, const char* edx);
    void (*fatal)(uint32_t id);
};
extern "C" void d2l_182d0_core(const D2l182d0Globals* g, const uint32_t t[6], uint32_t fmt, const D2l182d0Seams* s) {
    int32_t& n = *g->count;
    char buf[100];
    uint32_t h;
#define STEP(i) if (n >= 0x40) { s->fatal(0x624); return; } h = s->win10098(t[i]); g->table[n] = h; n += 1;
    s->f1b310();
    STEP(0)
    STEP(1) *g->g25e68 = h; s->win10174(h, 3);
    STEP(2) *g->g25e64 = h;
    int32_t pct = s->vt4c();
    if (pct > 100) pct = 100;
    STEP(3) s->win10000(h, s->lang10003(0x143a));
    STEP(4) s->win10000(h, s->lang10003(0x143b));
    STEP(5)
    int32_t w = s->win10112(*g->g25e68);
    s->win10161(*g->g25e68, (int32_t)(((int64_t)w * pct) / 100));
    s->sprintf3(buf, fmt, pct);
    s->win10022(*g->g25e64, buf);
#undef STEP
}
