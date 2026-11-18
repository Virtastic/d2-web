#include <cstdint>
// 1.13c D2Launch+0x19290 -- BNET login screen. g25E40 := 1. A live realm connection (vtable slot +0x68) with g25EA4 set drops it:
// g25EB0 := 1, state bytes 0x35D/0x37D := 0, OutputDebugStringA, D2MCPClient#10018, vtable slot +0x10; then 0x17DF0 and 0x1EE10
// (zero returns here). Otherwise the screen is built: g254FC/g2529C/g25518/g25504 := 0, 0x1B310, then eleven templates pushed onto
// the 0x40-slot table (full table is fatal, error 0x624, checked before every push). 3 and 4 get D2Lang strings 0x1468 / 0x1469 through
// D2Win#10020; unless Fog#10227, 0x12B10(EAX = 0x1F) and string 0x1454 through D2Win#10000; 5 string 0x1455 through D2Win#10000;
// 6 and 7 (g254FC, g2529C) get D2Win#10202(h, 0x10 / 0xD), D2Win#10010(h, cb 0x12330) and D2Win#10170(h, cb 0x126C0 / 0x126B0);
// Storm#422 ("Diablo II"-style key) decides which of the two D2Win#10053 selects (10185 first on success); two D2Win#10146
// cross-links; 10 stored in g25504; 0x122C0; with g25E6C set 0x12B10(EAX = 0x28) and string 0x2B62 through D2Win#10020;
// finally D2sound#10035 and, if nonzero, D2sound#10045(1).
struct D2l19290Globals {
    int32_t *g25e40, *g25eb0, *g25ea4, *count, *g25e6c;
    uint32_t *g254fc, *g2529c, *g25518, *g25504;
    uint32_t* table;
};
struct D2l19290Seams {
    int32_t (*vt68)();
    void (*dbgStr)(uint32_t);
    void (*mcp10018)();
    void (*vt10)(uint32_t);
    void (*f17df0)();
    int32_t (*fee10)();
    void (*f1b310)();
    uint32_t (*win10098)(uint32_t tmpl);
    uint32_t (*lang10003)(uint32_t id);
    void (*win10020)(uint32_t ecx, uint32_t edx);
    uint32_t (*f12b10)(uint32_t eax);
    int32_t (*fog10227)();
    void (*win10000)(uint32_t ecx, uint32_t edx);
    void (*win10202)(uint32_t ecx, uint32_t edx);
    void (*win10010)(uint32_t ecx, uint32_t edx);
    void (*win10170)(uint32_t ecx, uint32_t edx);
    int32_t (*storm422)(uint32_t app, uint32_t key, char* buf);
    void (*win10185)(uint32_t ecx, const char* edx);
    void (*win10053)(uint32_t sel);
    void (*win10146)(uint32_t ecx, uint32_t edx);
    void (*f122c0)();
    int32_t (*snd10035)();
    void (*snd10045)(uint32_t ecx);
    void (*fatal)(uint32_t id);
};
extern "C" void d2l_19290_core(const D2l19290Globals* g, uint32_t vt, uint8_t* state, const uint32_t t[11], uint32_t dbgText, uint32_t app,
                               uint32_t key, uint32_t cb330, uint32_t cb6c0, uint32_t cb6b0, const D2l19290Seams* s) {
    *g->g25e40 = 1;
    if (s->vt68()) {
        if (*g->g25ea4 != 0) {
            if (vt) {
                *g->g25eb0 = 1;
                state[0x35d] = 0; state[0x37d] = 0;
                s->dbgStr(dbgText);
                s->mcp10018();
                s->vt10(vt);
            }
            goto after;
        }
        goto build;
    }
after:
    s->f17df0();
    if (s->fee10() == 0) return;
build:
    *g->g254fc = 0; *g->g2529c = 0; *g->g25518 = 0; *g->g25504 = 0;
    s->f1b310();
    int32_t& n = *g->count;
    uint32_t h = 0;
#define STEP(i) if (n >= 0x40) { s->fatal(0x624); return; } h = s->win10098(t[i]); g->table[n] = h; n += 1;
    STEP(0)
    STEP(1)
    STEP(2) s->win10020(h, s->lang10003(0x1468));
    STEP(3) s->win10020(h, s->lang10003(0x1469));
    if (s->fog10227() == 0) { uint32_t b = s->f12b10(0x1f); s->win10000(b, s->lang10003(0x1454)); }
    STEP(4) s->win10000(h, s->lang10003(0x1455));
    STEP(5) *g->g254fc = h; s->win10202(h, 0x10); s->win10010(*g->g254fc, cb330); s->win10170(*g->g254fc, cb6c0);
    STEP(6) *g->g2529c = h; s->win10202(h, 0xd); s->win10010(*g->g2529c, cb330); s->win10170(*g->g2529c, cb6b0);
    uint32_t sel = *g->g254fc;
    char buf[0x400];
    if (s->storm422(app, key, buf)) { s->win10185(*g->g254fc, buf); sel = *g->g2529c; }
    s->win10053(sel);
    s->win10146(*g->g254fc, *g->g2529c);
    s->win10146(*g->g2529c, *g->g254fc);
    STEP(7)
    STEP(8)
    STEP(9) *g->g25504 = h;
    STEP(10)
#undef STEP
    s->f122c0();
    if (*g->g25e6c) { uint32_t b = s->f12b10(0x28); s->win10020(b, s->lang10003(0x2b62)); }
    if (s->snd10035()) s->snd10045(1);
}
