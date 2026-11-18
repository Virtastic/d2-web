#include <cstdint>
// 1.13c D2Launch+0x17B30 -- "disconnect from BNET" screen: g25E40 := 1; when a realm vtable (g256C4) is set, clear the connected
// flags (g25EB0 := 1, state bytes 0x35D and 0x37D := 0), OutputDebugStringA("Disconnect from BNET\n"), D2MCPClient#10018 and the
// vtable's slot +0x10 (EAX = vtable). Then clear the screen list (0x15D30), load the sky palette (0x12BF0: stack = name, EAX = the
// second string) and push ten templates onto the 0x40-slot table (a full table is fatal, error 0x624 -- checked before every push):
// 1 plain; 2 + D2Lang string 0x13FD through D2Win#10020(handle, string); 3 stored in g252A0, string 0x1401; 4 g25514; 5 g25510;
// 6 g25AC8, string 0x1402; 7 g25ACC, string 0x1403, then D2Win#10131(g25AC8, 0) and (g25ACC, 0); 8 D2Win#10031(handle); 9 g252AC,
// D2Win#10069(handle) and D2Win#10201(g252AC); 10 g254F8. Ends with a tail jump to 0x12560.
struct D2l17b30Globals {
    int32_t *g25e40, *g25eb0, *count;
    uint32_t *g252a0, *g25514, *g25510, *g25ac8, *g25acc, *g252ac, *g254f8;
    uint32_t* table;
};
struct D2l17b30Seams {
    void (*dbgStr)(uint32_t str);
    void (*mcp10018)();
    void (*vt10)(uint32_t vt);
    void (*f15d30)();
    void (*f12bf0)(uint32_t eax, uint32_t name);
    uint32_t (*win10098)(uint32_t tmpl);
    uint32_t (*lang10003)(uint32_t id);
    void (*win10020)(uint32_t ecx, uint32_t edx);
    void (*win10131)(uint32_t ecx, uint32_t edx);
    void (*win10031)(uint32_t h);
    void (*win10069)(uint32_t ecx);
    void (*win10201)(uint32_t ecx);
    void (*f12560)();
    void (*fatal)(uint32_t id);
};
extern "C" void d2l_17b30_core(const D2l17b30Globals* g, uint32_t vt, uint8_t* state, const uint32_t t[10], uint32_t dbgText,
                               uint32_t palName, uint32_t palEax, const D2l17b30Seams* s) {
    *g->g25e40 = 1;
    if (vt) {
        *g->g25eb0 = 1;
        state[0x35d] = 0; state[0x37d] = 0;
        s->dbgStr(dbgText);
        s->mcp10018();
        s->vt10(vt);
    }
    s->f15d30();
    s->f12bf0(palEax, palName);
    int32_t& n = *g->count;
    uint32_t h = 0;
#define STEP(i) if (n >= 0x40) { s->fatal(0x624); return; } h = s->win10098(t[i]); g->table[n] = h; n += 1;
    STEP(0)
    STEP(1) s->win10020(h, s->lang10003(0x13fd));
    STEP(2) *g->g252a0 = h; s->win10020(*g->g252a0, s->lang10003(0x1401));
    STEP(3) *g->g25514 = h;
    STEP(4) *g->g25510 = h;
    STEP(5) *g->g25ac8 = h; s->win10020(*g->g25ac8, s->lang10003(0x1402));
    STEP(6) *g->g25acc = h; s->win10020(*g->g25acc, s->lang10003(0x1403)); s->win10131(*g->g25ac8, 0); s->win10131(*g->g25acc, 0);
    STEP(7) s->win10031(h);
    STEP(8) *g->g252ac = h; s->win10069(h); s->win10201(*g->g252ac);
    STEP(9) *g->g254f8 = h;
#undef STEP
    s->f12560();
}
