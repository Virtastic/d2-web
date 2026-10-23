// src/core/d2win/d2win_batch_b51.cpp -- 1.13c D2Win.dll small functions (alpha batch lane, no spec).
// Transcribed from the cached decompile. Retail globals/callees arrive through an env struct; offsets
// in comments are the retail 32-bit layout (pointers widen).
#include <cstdint>
#include <emscripten.h>

struct D2WinCtl {                  // control header, common prefix
    uint32_t type;                 // +0
    uint32_t pad[7];
    void* cb20;                    // +0x20 (index 8)
    uint32_t pad2[6];
    D2WinCtl* next;                // +0x3C
    uint32_t pad3[2];
    uint32_t v48;                  // +0x48 (type 1 only)
};
struct D2WinB51Env {
    void (*fatal)(uint32_t stringId);        // Ordinal_10265 + Ordinal_10024("") then _exit(-1)
    D2WinCtl** listHead;                     // 0x6F9014A0
    D2WinCtl** g14b0; D2WinCtl** g14ac;      // 0x6F9014B0 / 0x6F9014AC
    D2WinCtl** g14b8; uint32_t* g9cec;       // 0x6F9014B8 / 0x6F9A9CEC
    D2WinCtl** g14b4;                        // 0x6F9014B4
    void (*freeCtl)(D2WinCtl*);              // 0x17A00
    void (*sub_B50)(uint32_t, int32_t, int32_t); // 0xBB50
    uint32_t* g_ffd30;                       // 0x6F8FFD30
};

// 1.13c D2Win+0x17AF0
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_17af0(const D2WinB51Env* e) { *e->g_ffd30 = 0; return 1; }

// 1.13c D2Win+0x16470
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_16470(D2WinCtl* c, void* cb6360, const D2WinB51Env* e) {
    if (c->type != 8) e->fatal(0x45);
    c->cb20 = cb6360;
}

// 1.13c D2Win+0x136e0
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_136e0(D2WinCtl* c, uint32_t v, const D2WinB51Env* e) {
    if (!c) e->fatal(0x445);
    if (c->type != 1) e->fatal(0x448);
    c->v48 = v;
}

// 1.13c D2Win+0xBD00
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_bd00(uint32_t p1, int32_t p2, int32_t p3, int32_t p4, const D2WinB51Env* e) {
    int32_t t = p4 ? 5 : 4, i;
    if (p2 < t) i = 0;
    else if (p2 < (p4 ? 10 : 8)) i = 1;
    else i = (p2 >= (p4 ? 15 : 12)) ? 3 : 2;
    if (p3 && i) i += 3;
    e->sub_B50(p1, i, p4);
}

// 1.13c D2Win+0x11E80
// lineCount source: string of UTF-16 code units; fontByte = byte at (font table[curFont].ptr + 10).
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2win_sub_11e80(const uint16_t* s, uint32_t fontByte, int32_t scale, const D2WinB51Env* e) {
    if (!s) e->fatal(0x25e);
    int32_t n = 1;
    for (const uint16_t* p = s; *p; ++p) if (*p == 10) ++n;
    return (int32_t)(fontByte * (uint32_t)scale * (uint32_t)n) / 10;
}

// 1.13c D2Win+0x17fc0
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_17fc0(D2WinCtl* c, const D2WinB51Env* e) {
    if (*e->g14b0 == c) *e->g14b0 = nullptr;
    if (*e->g14ac == c) *e->g14ac = nullptr;
    if (*e->g14b8 == c) { *e->g14b8 = nullptr; *e->g9cec = 0; }
    if (*e->g14b4 == c) *e->g14b4 = nullptr;
    D2WinCtl* h = *e->listHead;
    if (!h) return 0;
    if (h == c) { *e->listHead = c->next; e->freeCtl(c); return 1; }
    D2WinCtl* prev = h;
    D2WinCtl* cur = h->next;
    if (cur) {
        while (cur != c) {
            prev = cur; cur = cur->next;
            if (!cur) return 1;
        }
        prev->next = cur->next;
        e->freeCtl(c);
    }
    return 1;
}
