// Shared seams for the D2Launch screen builders (0x14A20, 0x14C60, 0x15090, 0x15230, 0x15400, 0x155B0): the 0x40-slot control table
// (count g25E34, slots g253C0) filled by D2Win#10098 from templates, with hidden-register callees 0x12A70 (BL = 0) and 0x12BA0 (EAX = index).
#pragma once
#include <cstdint>
struct D2lScr {
    uint32_t (*win10098)(uint32_t tmpl);
    void (*win10196)(uint32_t h, uint32_t v);
    void (*win10001)(uint32_t h, uint32_t v);
    void (*win10050)(uint32_t* slot);
    void (*win10020)(uint32_t ecx, uint32_t edx);
    void (*win10167)(uint32_t ecx, uint32_t edx);
    uint32_t (*lang10003)(uint32_t ecx);
    void (*f52a70)(uint32_t h, uint32_t a, uint32_t b);   // stack (h, a, b), ret 0xC, BL = 0
    uint32_t (*f52ba0)(uint32_t idx);                      // EAX = idx
    void (*fatal)();
};
struct D2lScrG { int32_t* count; uint32_t* table; uint32_t* g25e40; uint32_t* g25e60; uint32_t* g2552c; };
static inline void d2lScrFree(const D2lScrG* g, const D2lScr* s) {
    for (int32_t i = 0; i < *g->count; i++) s->win10050(&g->table[i]);
    *g->count = 0;
}
// 0x40-slot check (fatal 0x60D when full), then D2Win#10098(tmpl) stored at table[count++]
static inline uint32_t d2lScrPush(const D2lScrG* g, const D2lScr* s, uint32_t tmpl, bool* ok) {
    if (*g->count >= 0x40) { s->fatal(); *ok = false; return 0; }
    uint32_t h = s->win10098(tmpl);
    g->table[(*g->count)++] = h;
    return h;
}
#define D2L_PUSH(h, t) uint32_t h = d2lScrPush(g, s, (t), &ok); if (!ok) return
