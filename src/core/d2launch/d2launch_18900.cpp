#include <cstdint>
// 1.13c D2Launch+0x18900 -- realm-select screen (stdcall, one ignored argument, returns 1). D2sound#10023(0xB4), #10014, #10061(0xFF),
// 0x1B310. The "Aux Battle.net" setting is read with Storm#422(app, key, 0, buf, 0x64): when present it is parsed with
// sscanf(buf, fmt, &a, &b, &c, &d) (d is pre-set to 0x22, and only its low byte matters) and classified: bit 0 gives 7, the sign 6,
// 0x10 gives 5, 8 gives 4, 0x40 gives 3, else (bit 2) + 1; when absent the class is 1 and the default address (216.148.246.34) is
// formatted and stored with Storm#425(app, key, 0, buf). With Fog#10227 set only 0x167B0(class) runs. Else eight templates are
// pushed onto the 0x40-slot table (fatal 0x624 when full, checked before each): the second gets D2Lang string 0x13FA through
// D2Win#10020; after the seventh, D2Win#10147(h, 0) runs on the 4th..7th handles counted from the class (class 1 or above 7: all
// four; 2: the last three; 3: two; 4: one; 5 to 7: none).
struct D2l18900Globals { int32_t* count; uint32_t* table; };
struct D2l18900Seams {
    void (*snd10023)(uint32_t ecx);
    void (*snd10014)();
    void (*snd10061)(uint32_t ecx);
    void (*f1b310)();
    int32_t (*storm422)(uint32_t app, uint32_t key, char* buf);
    void (*scan)(const char* buf, uint32_t fmt, uint32_t out[4]);
    void (*fmtDefault)(char* buf, uint32_t fmt);
    void (*storm425)(uint32_t app, uint32_t key, const char* buf);
    int32_t (*fog10227)();
    void (*f167b0)(uint32_t cls);
    uint32_t (*win10098)(uint32_t tmpl);
    uint32_t (*lang10003)(uint32_t id);
    void (*win10020)(uint32_t ecx, uint32_t edx);
    void (*win10147)(uint32_t ecx, uint32_t edx);
    void (*fatal)(uint32_t id);
};
extern "C" void d2l_18900_core(const D2l18900Globals* g, const uint32_t t[8], uint32_t app, uint32_t key, uint32_t scanFmt, uint32_t defFmt,
                               const D2l18900Seams* s) {
    char buf[0x64];
    uint32_t o[4] = { 0, 0, 0, 0x22 };
    uint8_t cls;
    s->snd10023(0xb4);
    s->snd10014();
    s->snd10061(0xff);
    s->f1b310();
    if (s->storm422(app, key, buf)) {
        s->scan(buf, scanFmt, o);
        uint8_t al = (uint8_t)o[3];
        if (al & 1) cls = 7;
        else if (al & 0x80) cls = 6;
        else if (al & 0x10) cls = 5;
        else if (al & 8) cls = 4;
        else if (al & 0x40) cls = 3;
        else cls = (al & 4) ? 2 : 1;
    } else {
        cls = 1;
        s->fmtDefault(buf, defFmt);
        s->storm425(app, key, buf);
    }
    if (s->fog10227()) { s->f167b0(cls); return; }
    int32_t& n = *g->count;
    uint32_t h[8];
    for (int i = 0; i < 7; i++) {
        if (n >= 0x40) { s->fatal(0x624); return; }
        h[i] = s->win10098(t[i]); g->table[n] = h[i]; n += 1;
        if (i == 1) s->win10020(h[i], s->lang10003(0x13fa));
    }
    switch (cls) {
        default: s->win10147(h[3], 0); /* fall through */
        case 2: s->win10147(h[4], 0); /* fall through */
        case 3: s->win10147(h[5], 0); /* fall through */
        case 4: s->win10147(h[6], 0); /* fall through */
        case 5: case 6: case 7: break;
    }
    if (n >= 0x40) { s->fatal(0x624); return; }
    h[7] = s->win10098(t[7]); g->table[n] = h[7]; n += 1;
}
