// 1.13c D2Launch+0x14290 (sub_14290). stdcall, one arg (ret 4): the character-select click handler for the class cels.
//  ev := thunk 0x9964 (ECX = p[0]); the class cels DAT_6fa654dc.. (7 with the expansion, 5 without) must all pass thunk 0x9982 (ECX = cel)
//  or the handler returns 0. The old selection DAT_6fa64d94 is cleared to -1; a real old class stops its sounds (0x9A5A) and plays its
//  "deselect" wav (0x9A54). p[0] picks the class (121B0 for the five classic ones, 141B0/141F0 for druid/assassin); a new class on ev 1
//  plays its "select" wav. The selection is stored unless the class did not change (or no class) on ev 2/3/4; 11AA0 marks it in the
//  screen block; then the buttons are enabled/disabled (0x987A), 0x988C, the expansion pair and the 0x1EB flags. Returns 1.
#include <cstdint>

struct D2l14290Seams {
    uint32_t (*evt9964)(uint32_t cel);
    uint32_t (*isExp)();
    uint32_t (*ok9982)(uint32_t cel);
    void (*stop9a5a)();
    void (*play9a54)(const char* path);
    void (*cls121b0)(uint32_t p3, uint32_t a, uint32_t b);
    void (*cls141b0)(uint32_t* p);
    void (*cls141f0)(uint32_t* p);
    void (*mark11aa0)(char cls);
    void (*enable987a)(uint32_t h, uint32_t on);
    void (*set988c)(uint32_t h);
    void (*focus9868)(uint32_t h, uint32_t on);
};

// The module globals the handler reads, gathered by the caller.
struct D2l14290G {
    int32_t* sel;            // DAT_6fa64d94
    uint32_t* cel;           // DAT_6fa654d8 .. DAT_6fa654f4 (8 dwords; [1..7] are the class cels)
    uint32_t* b508;          // DAT_6fa65508
    uint32_t* b520;          // DAT_6fa65520 .. DAT_6fa6553c (8 dwords)
    uint32_t b298, b2a4, b2b0, b3b8;
    int32_t ad4;             // DAT_6fa65ad4
    uint32_t eb0;            // DAT_6fa65eb0
    uint8_t* st;             // DAT_6fa65abc
};

static const char* const kDeselect[7] = {
    "data\\global\\sfx\\cursor\\intro\\amazon deselect.wav", "data\\global\\sfx\\cursor\\intro\\sorceress deselect.wav",
    "data\\global\\sfx\\cursor\\intro\\necromancer deselect.wav", "data\\global\\sfx\\cursor\\intro\\paladin deselect.wav",
    "data\\global\\sfx\\cursor\\intro\\barbarian deselect.wav", "data\\global\\sfx\\cursor\\intro\\druid deselect.wav",
    "data\\global\\sfx\\cursor\\intro\\assassin deselect.wav" };
static const char* const kSelect[7] = {
    "data\\global\\sfx\\cursor\\intro\\amazon select.wav", "data\\global\\sfx\\cursor\\intro\\sorceress select.wav",
    "data\\global\\sfx\\cursor\\intro\\necromancer select.wav", "data\\global\\sfx\\cursor\\intro\\paladin select.wav",
    "data\\global\\sfx\\cursor\\intro\\barbarian select.wav", "data\\global\\sfx\\cursor\\intro\\druid select.wav",
    "data\\global\\sfx\\cursor\\intro\\assassin select.wav" };

extern "C" uint32_t d2l_14290(uint32_t* p, const D2l14290G* g, const D2l14290Seams* s) {
    uint32_t ev = s->evt9964(p[0]);
    uint32_t n = s->isExp() ? 7 : 5;
    for (uint32_t i = 0; i < n; i++)
        if (!s->ok9982(g->cel[1 + i])) return 0;
    int32_t old = *g->sel;
    *g->sel = -1;
    if (old != -1) {
        s->stop9a5a();
        if ((uint32_t)old <= 6) s->play9a54(kDeselect[old]);
    }
    // cel[1..7]: 654dc barbarian(4), 654e0 necromancer(2), 654e4 amazon(0), 654e8 sorceress(1), 654ec paladin(3), 654f0 druid, 654f4 assassin
    int32_t idx = -1;
    uint32_t k = p[0];
    if (k == g->cel[1]) { idx = 4; s->cls121b0(p[3], 0xfa7, 0x140a); }
    else if (k == g->cel[2]) { idx = 2; s->cls121b0(p[3], 0xfa9, 0x1409); }
    else if (k == g->cel[3]) { idx = 0; s->cls121b0(p[3], 0xfab, 0x1408); }
    else if (k == g->cel[4]) { idx = 1; s->cls121b0(p[3], 0xfaa, 0x140b); }
    else if (k == g->cel[5]) { idx = 3; s->cls121b0(p[3], 0xfa8, 0x140c); }
    else if (k == g->cel[6]) { idx = 5; s->cls141b0(p); }
    else if (k == g->cel[7]) { idx = 6; s->cls141f0(p); }
    if (idx != old && ev == 1) {
        if ((uint32_t)idx <= 6) s->play9a54(kSelect[idx]);
        *g->sel = idx;
    } else if (ev != 2 && ev != 3 && ev != 4) {
        *g->sel = idx;
    }
    s->mark11aa0((char)*g->sel);
    uint32_t* b = g->b520;   // b[0] 65520, b[1] 65524, b[2] 65528, b[5] 65534, b[6] 65538, b[7] 6553c
    if (*g->sel == -1 && ev != 1) {
        s->enable987a(*g->b508, 0); s->enable987a(b[7], 0); s->enable987a(b[0], 0); s->enable987a(b[5], 0);
        s->enable987a(g->b3b8, 0); s->enable987a(g->b298, 0);
        if (g->ad4 == 1) { s->enable987a(b[2], 0); s->enable987a(g->b2b0, 0); }
        if (s->isExp()) { s->enable987a(g->cel[0], 0); s->enable987a(b[6], 0); s->enable987a(b[1], 0); }
        s->set988c(0);
        if (g->ad4 == 1) s->enable987a(g->b2a4, 1);
    } else {
        s->enable987a(*g->b508, 1); s->enable987a(b[7], 1);
        if (g->eb0) { s->enable987a(b[0], 1); s->enable987a(b[5], 1); }
        if (g->ad4 == 1) { s->enable987a(b[2], 1); s->enable987a(g->b2b0, 1); }
        s->enable987a(g->b3b8, 1);
        s->set988c(g->b3b8);
        s->enable987a(g->b298, 1);
        if (g->ad4 == 1) s->enable987a(g->b2a4, 0);
    }
    if (s->isExp()) {
        int32_t c = *g->sel;
        if (c == 6 || c == 5) {
            s->enable987a(g->cel[0], 1); s->enable987a(b[6], 0); s->enable987a(b[1], 1);
            g->st[0x1eb] |= 0x20;
        } else if (c != -1) {
            s->enable987a(g->cel[0], 1); s->enable987a(b[6], 1); s->enable987a(b[1], 0);
            s->focus9868(b[6], 1);
            g->st[0x1eb] |= 0x20;
        }
    }
    if (g->ad4 == 1) { s->focus9868(g->b2b0, 1); g->st[0x1eb] |= 0x40; }
    return 1;
}
